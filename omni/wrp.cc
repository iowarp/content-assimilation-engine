#include "format/format_factory.h"
#include "repo/filesystem_repo_omni.h"
#include "repo/repo_factory.h"
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <pwd.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <yaml-cpp/yaml.h>

using namespace cae;

std::string ExpandPath(const std::string &path) {
  if (path.empty() || path[0] != '~') {
    return path;
  }

  if (path.length() == 1 || path[1] == '/') {
    // ~/... case
    const char *home = getenv("HOME");
    if (!home) {
      struct passwd *pw = getpwuid(getuid());
      if (pw) {
        home = pw->pw_dir;
      }
    }
    if (home) {
      return std::string(home) + (path.length() > 1 ? path.substr(1) : "");
    }
  }

  // If we can't expand, return original path
  return path;
}

struct OmniJobConfig {
  std::string name;
  int max_scale;

  struct DataEntry {
    std::string path;
    std::vector<size_t> range;
    size_t offset;
    size_t size;
    std::vector<std::string> description;
    std::string hash;

    DataEntry() : offset(0), size(0) {}
  };

  std::vector<DataEntry> data_entries;

  OmniJobConfig() : max_scale(100) {}
};

OmniJobConfig ParseOmniFile(const std::string &yaml_file) {
  OmniJobConfig config;

  try {
    YAML::Node yaml = YAML::LoadFile(yaml_file);

    if (yaml["name"]) {
      config.name = yaml["name"].as<std::string>();
    }

    if (yaml["max_scale"]) {
      config.max_scale = yaml["max_scale"].as<int>();
    }

    if (yaml["data"]) {
      const YAML::Node &data_node = yaml["data"];
      for (const auto &entry : data_node) {
        OmniJobConfig::DataEntry data_entry;

        if (entry["path"]) {
          data_entry.path = ExpandPath(entry["path"].as<std::string>());
        }

        if (entry["range"]) {
          const YAML::Node &range_node = entry["range"];
          for (const auto &val : range_node) {
            data_entry.range.push_back(val.as<size_t>());
          }
        }

        if (entry["offset"]) {
          data_entry.offset = entry["offset"].as<size_t>();
        }

        if (entry["size"]) {
          data_entry.size = entry["size"].as<size_t>();
        }

        if (entry["description"]) {
          const YAML::Node &desc_node = entry["description"];
          for (const auto &desc : desc_node) {
            data_entry.description.push_back(desc.as<std::string>());
          }
        }

        if (entry["hash"]) {
          data_entry.hash = entry["hash"].as<std::string>();
        }

        // If size is not specified (0), automatically detect file size
        if (data_entry.size == 0 && !data_entry.path.empty()) {
          FilesystemRepoClient fs_client;
          size_t file_size = fs_client.GetFileSize(data_entry.path);
          if (file_size > 0) {
            // Calculate size as file_size - offset to read from offset to end
            // of file
            if (data_entry.offset < file_size) {
              data_entry.size = file_size - data_entry.offset;
              std::cout << "Auto-detected size for " << data_entry.path << ": "
                        << data_entry.size << " bytes (from offset "
                        << data_entry.offset << " to end of file)" << std::endl;
            } else {
              std::cerr << "Warning: Offset " << data_entry.offset
                        << " is beyond file size " << file_size << " for "
                        << data_entry.path << std::endl;
            }
          } else {
            std::cerr << "Warning: Could not determine file size for "
                      << data_entry.path << std::endl;
          }
        }

        // Derive range from offset and size if range is not specified
        if (data_entry.range.empty() && data_entry.size > 0) {
          data_entry.range.push_back(data_entry.offset);
          data_entry.range.push_back(data_entry.offset + data_entry.size);
        }

        config.data_entries.push_back(data_entry);
      }
    }

  } catch (const YAML::Exception &e) {
    std::cerr << "YAML parsing error: " << e.what() << std::endl;
    throw;
  }

  return config;
}

std::string BuildMpiCommand(const OmniJobConfig::DataEntry &entry, int nprocs) {
  std::ostringstream cmd;

  // Build description string
  std::string description;
  if (!entry.description.empty()) {
    std::ostringstream desc_stream;
    for (size_t i = 0; i < entry.description.size(); ++i) {
      if (i > 0)
        desc_stream << ",";
      desc_stream << entry.description[i];
    }
    description = desc_stream.str();
  }

  // Construct MPI command with environment forwarding
  cmd << "mpirun -x LD_PRELOAD"; // Forward LD_PRELOAD explicitly

  // Forward other important environment variables
  const char *important_env_vars[] = {"PATH",
                                      "HOME",
                                      "USER",
                                      "TMPDIR",
                                      "LD_LIBRARY_PATH",
                                      "PYTHONPATH",
                                      "CUDA_VISIBLE_DEVICES",
                                      nullptr};

  for (int i = 0; important_env_vars[i] != nullptr; ++i) {
    if (getenv(important_env_vars[i])) {
      cmd << " -x " << important_env_vars[i];
    }
  }

  cmd << " -np " << nprocs;
  cmd << " ./wrp_binary_format_mpi";
  cmd << " \"" << entry.path << "\"";
  cmd << " " << entry.offset;
  cmd << " " << entry.size;

  if (!description.empty()) {
    cmd << " \"" << description << "\"";
  }

  if (!entry.hash.empty()) {
    cmd << " \"" << entry.hash << "\"";
  }

  return cmd.str();
}

void ProcessDataEntry(const OmniJobConfig::DataEntry &entry, int nprocs) {
  std::cout << "\n" << std::string(50, '=') << std::endl;
  std::cout << "Processing Data Entry" << std::endl;
  std::cout << std::string(50, '=') << std::endl;
  std::cout << "File: " << entry.path << std::endl;
  std::cout << "Offset: " << entry.offset << " bytes" << std::endl;
  std::cout << "Size: " << entry.size << " bytes" << std::endl;
  std::cout << "MPI Processes: " << nprocs << std::endl;

  // Build and execute MPI command
  std::string mpi_command = BuildMpiCommand(entry, nprocs);
  std::cout << "Executing: " << mpi_command << std::endl;
  std::cout << std::string(50, '-') << std::endl;

  int result = system(mpi_command.c_str());

  std::cout << std::string(50, '-') << std::endl;
  if (result == 0) {
    std::cout << "✓ Successfully completed processing " << entry.path
              << std::endl;
  } else {
    std::cerr << "✗ Failed to process " << entry.path
              << " (exit code: " << result << ")" << std::endl;
  }
}

int main(int argc, char *argv[]) {
  // Initialize MPI for the main orchestrator
  MPI_Init(&argc, &argv);

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <omni_yaml_file>" << std::endl;
    MPI_Finalize();
    return 1;
  }

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  try {
    // Parse the OMNI YAML file
    OmniJobConfig config = ParseOmniFile(argv[1]);

    if (rank == 0) {
      std::cout << "OMNI Content Assimilation Engine" << std::endl;
      std::cout << "=================================" << std::endl;
      std::cout << "Job: " << config.name << std::endl;
      std::cout << "Max scale: " << config.max_scale << std::endl;
      std::cout << "Number of data entries: " << config.data_entries.size()
                << std::endl;

      // Check if wrp_binary_format_mpi exists
      if (system("test -x ./wrp_binary_format_mpi") != 0) {
        std::cerr
            << "Error: wrp_binary_format_mpi binary not found or not executable"
            << std::endl;
        std::cerr
            << "Make sure it's built and available in the current directory"
            << std::endl;
        MPI_Finalize();
        return 1;
      }

      // Process each data entry
      for (size_t i = 0; i < config.data_entries.size(); ++i) {
        const auto &entry = config.data_entries[i];

        std::cout << "\nData Entry " << (i + 1) << "/"
                  << config.data_entries.size() << ": " << entry.path
                  << std::endl;

        // Use filesystem repo client to recommend scale
        FilesystemRepoClient fs_client;
        int nprocs, nthreads;
        fs_client.RecommendScaleForFile(entry.path, config.max_scale, nprocs,
                                        nthreads);

        // Process the data entry using MPI subprocess
        ProcessDataEntry(entry, nprocs);
      }

      std::cout << "\n" << std::string(50, '=') << std::endl;
      std::cout << "✓ All data entries processed successfully!" << std::endl;
      std::cout << std::string(50, '=') << std::endl;
    }

  } catch (const std::exception &e) {
    if (rank == 0) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
    MPI_Finalize();
    return 1;
  }

  MPI_Finalize();
  return 0;
}