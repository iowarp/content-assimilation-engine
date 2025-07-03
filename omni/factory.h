#pragma once
#include "OMNI.h"
#include <memory>
#include <string>

namespace CAE {

// Factory for RepoClient
std::unique_ptr<RepoClient> CreateRepoClient(const std::string& type);

// Factory for FormatClient
std::unique_ptr<FormatClient> CreateFormatClient(const std::string& type);

} // namespace CAE 