#ifndef CAE_ADAPTERS_CONFIG_MANAGER_H_
#define CAE_ADAPTERS_CONFIG_MANAGER_H_

#include "adapters/adapter_types.h"
#include "config_client_default.h"
#include "hermes/hermes.h"
#include <filesystem>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace cae {

class Constant {
public:
  /** CAE conf environment variable */
  CONST_T char *kCaeConf = "IOWARP_CAE_CONF";

  /** CAE adapter mode environment variable */
  CONST_T char *kAdapterMode = "IOWARP_CAE_ADAPTER_MODE";

  /** Filesystem page size environment variable */
  CONST_T char *kPageSize = "IOWARP_CAE_PAGE_SIZE";
};

/**< A path is included */
static inline const bool do_include = true;
/**< A path is excluded */
static inline const bool do_exclude = false;

/** Stores information about path inclusions and exclusions */
struct UserPathInfo {
  std::regex regex_;  /**< The regex to match the path */
  std::string path_;  /**< The path the user specified */
  bool include_;      /**< Whether to track path. */
  bool is_directory_; /**< Whether the path is a file or directory */

  /** Default constructor */
  UserPathInfo() = default;

  static std::string ToRegex(const std::string &path) {
    std::string regex_pattern = "^";
    for (char c : path) {
      if (c == '.') {
        regex_pattern += "\\.";
      } else if (c == '/') {
        regex_pattern += "\\/";
      } else if (c == '*') {
        regex_pattern += ".*";
      } else {
        regex_pattern += c;
      }
    }
    return regex_pattern;
  }

  /** Emplace Constructor */
  UserPathInfo(const std::string &path, bool include, bool is_directory)
      : path_(path), include_(include), is_directory_(is_directory) {
    std::string regex_pattern = ToRegex(path);
    if (is_directory) {
      regex_pattern += ".*";
    }
    regex_ = std::regex(regex_pattern);
  }

  /** Detect if a path matches the input path */
  bool Match(const std::string &abs_path) {
    return std::regex_match(abs_path, regex_) ||
           abs_path.rfind(path_) != std::string::npos;
  }
};

/**
 * Configuration used to initialize client
 * */
class ConfigurationManager : public hshm::BaseConfig {
public:
  /** The flushing mode to use */
  FlushingMode flushing_mode_;
  /** The set of paths to monitor or exclude, ordered by length */
  std::vector<UserPathInfo> path_list_;
  /** The default adapter config */
  AdapterObjectConfig base_adapter_config_;
  /** Per-object (e.g., file) adapter configuration */
  std::unordered_map<std::string, AdapterObjectConfig> adapter_config_;

public:
  ConfigurationManager() = default;

  void Init(std::string config_path = "") {
    if (config_path.empty()) {
      config_path = hshm::SystemInfo::Getenv(Constant::kCaeConf);
    }
    HILOG(kInfo, "Loading cae configuration: {}", config_path);
    LoadFromFile(config_path);
  }

  void LoadDefault() override { LoadText(kCaeClientDefaultConfigStr, false); }

  void SetBaseAdapterMode(AdapterMode mode) {
    base_adapter_config_.mode_ = mode;
  }

  AdapterMode GetBaseAdapterMode() { return base_adapter_config_.mode_; }

  AdapterObjectConfig &GetAdapterConfig(const std::string &path) {
    auto iter = adapter_config_.find(path);
    if (iter == adapter_config_.end()) {
      return base_adapter_config_;
    }
    return iter->second;
  }

  void SetAdapterConfig(const std::string &path, AdapterObjectConfig &conf) {
    adapter_config_.emplace(path, conf);
  }

  void CreateAdapterPathTracking(const std::string &path, bool include) {
    bool is_dir = stdfs::is_directory(path);
    path_list_.emplace_back(path, include, is_dir);
    std::sort(path_list_.begin(), path_list_.end(),
              [](const UserPathInfo &a, const UserPathInfo &b) {
                return a.path_.size() > b.path_.size();
              });
  }

  void SetAdapterPathTracking(const std::string &path, bool include) {
    for (auto &pth : path_list_) {
      if (pth.path_ == path) {
        pth.include_ = include;
        pth.is_directory_ = stdfs::is_directory(path);
        return;
      }
    }
    CreateAdapterPathTracking(path, include);
  }

  bool GetAdapterPathTracking(const std::string &path) {
    for (auto &pth : path_list_) {
      if (pth.path_ == path) {
        return pth.include_;
      }
    }
    return false;
  }

private:
  void ParseYAML(YAML::Node &yaml_conf) override {
    if (yaml_conf["base_adapter_mode"]) {
      std::string mode_env = hshm::SystemInfo::Getenv(Constant::kAdapterMode);
      if (mode_env.size() == 0) {
        base_adapter_config_.mode_ = AdapterModeConv::ToEnum(
            yaml_conf["base_adapter_mode"].as<std::string>());
      } else {
        base_adapter_config_.mode_ = AdapterModeConv::ToEnum(mode_env);
      }
    }
    if (yaml_conf["file_page_size"]) {
      std::string page_size_env = hshm::SystemInfo::Getenv(Constant::kPageSize);
      if (page_size_env.size() == 0) {
        base_adapter_config_.page_size_ = hshm::ConfigParse::ParseSize(
            yaml_conf["file_page_size"].as<std::string>());
      } else {
        base_adapter_config_.page_size_ =
            hshm::ConfigParse::ParseSize(page_size_env);
      }
    }
    if (yaml_conf["path_inclusions"]) {
      std::vector<std::string> inclusions;
      ParseVector<std::string>(yaml_conf["path_inclusions"], inclusions);
      for (auto &entry : inclusions) {
        entry = hshm::ConfigParse::ExpandPath(entry);
        SetAdapterPathTracking(std::move(entry), true);
      }
    }
    if (yaml_conf["path_exclusions"]) {
      std::vector<std::string> exclusions;
      ParseVector<std::string>(yaml_conf["path_exclusions"], exclusions);
      for (auto &entry : exclusions) {
        entry = hshm::ConfigParse::ExpandPath(entry);
        SetAdapterPathTracking(std::move(entry), false);
      }
    }
    if (yaml_conf["flushing_mode"]) {
      flushing_mode_ = FlushingModeConv::ToEnum(
          yaml_conf["flushing_mode"].as<std::string>());
      auto flush_mode_env = getenv("HERMES_FLUSH_MODE");
      if (flush_mode_env) {
        flushing_mode_ = FlushingModeConv::ToEnum(flush_mode_env);
      }
    }
    if (yaml_conf["file_adapter_configs"]) {
      for (auto node : yaml_conf["file_adapter_configs"]) {
        AdapterObjectConfig conf(base_adapter_config_);
        ParseAdapterConfig(node, conf);
      }
    }
  }

  void ParseAdapterConfig(YAML::Node &yaml_conf, AdapterObjectConfig &conf) {
    std::string path = yaml_conf["path"].as<std::string>();
    path = hshm::ConfigParse::ExpandPath(path);
    path = stdfs::absolute(path).string();
    if (yaml_conf["mode"]) {
      conf.mode_ = AdapterModeConv::ToEnum(yaml_conf["mode"].as<std::string>());
    }
    if (yaml_conf["page_size"]) {
      conf.page_size_ = hshm::ConfigParse::ParseSize(
          yaml_conf["page_size"].as<std::string>());
    }
    SetAdapterConfig(path, conf);
  }
};

/** Global configuration instance */
HSHM_DEFINE_GLOBAL_PTR_VAR_H((cae::ConfigurationManager), cae_conf)

/** Get the global configuration instance */
#define IOWARP_CAE_CONF                                                        \
  HSHM_GET_GLOBAL_PTR_VAR((cae::ConfigurationManager), cae::cae_conf)

/** Initialize CAE configuration */
static inline bool IOWARP_CAE_INIT() {
  HERMES_INIT();
  IOWARP_CAE_CONF->Init();
  return true;
}

} // namespace cae

#endif // CAE_ADAPTERS_CONFIG_MANAGER_H_