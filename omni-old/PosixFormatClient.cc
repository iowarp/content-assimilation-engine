#include "OMNI.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

namespace CAE {

void PosixFormatClient::Import(const FormatContext &ctx) {
    // Read bytes from file at offset and size
    int fd = open(ctx.filename_.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cerr << "Error: opening file " << ctx.filename_ << std::endl;
        return;
    }
    if (lseek(fd, ctx.offset_, SEEK_SET) == -1) {
        std::cerr << "Error: seeking file " << ctx.filename_ << std::endl;
        close(fd);
        return;
    }
    std::vector<unsigned char> buffer(ctx.size_);
    ssize_t total_bytes_read = 0;
    while (total_bytes_read < (ssize_t)ctx.size_) {
        ssize_t bytes_read = read(fd, buffer.data() + total_bytes_read, ctx.size_ - total_bytes_read);
        if (bytes_read == -1) {
            perror("Error reading file");
            close(fd);
            return;
        }
        if (bytes_read == 0) {
            std::cerr << "End of file reached after reading " << total_bytes_read << " bytes, expected " << ctx.size_ << "." << std::endl;
            close(fd);
            return;
        }
        total_bytes_read += bytes_read;
    }
    close(fd);
    std::cout << "Imported " << total_bytes_read << " bytes from " << ctx.filename_ << " at offset " << ctx.offset_ << std::endl;
    // You can add further processing here (e.g., send to backend, etc.)
}

void PosixFormatClient::Export(const FormatContext &ctx) {
    // Stub: implement as needed
    std::cout << "Export not implemented for PosixFormatClient." << std::endl;
}

} // namespace CAE 