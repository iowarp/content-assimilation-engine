#include "OMNI.h"
#include <iostream>

namespace CAE {

void HDF5FormatClient::Import(const FormatContext &ctx) {
    std::cout << "HDF5FormatClient::Import called for file: " << ctx.filename_ << std::endl;
    // Real HDF5 import logic would go here
}

void HDF5FormatClient::Export(const FormatContext &ctx) {
    std::cout << "HDF5FormatClient::Export called for file: " << ctx.filename_ << std::endl;
    // Real HDF5 export logic would go here
}

} // namespace CAE 