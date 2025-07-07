#ifndef CAE_FORMAT_PROGRESS_BAR_H_
#define CAE_FORMAT_PROGRESS_BAR_H_

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>

namespace cae {

class ProgressBar {
public:
  ProgressBar(const std::string &title, size_t total, int rank, int width = 50)
      : title_(title), total_(total), current_(0), width_(width), rank_(rank),
        start_time_(std::chrono::steady_clock::now()) {
    PrintBar(0);
  }

  void Update(size_t current) {
    current_ = current;
    PrintBar(CalculatePercentage());
  }

  void Finish() { PrintBar(100); }

private:
  double CalculatePercentage() const {
    return total_ == 0 ? 100 : (current_ * 100.0) / total_;
  }

  std::string FormatTime(double seconds) const {
    int hours = static_cast<int>(seconds) / 3600;
    int minutes = (static_cast<int>(seconds) % 3600) / 60;
    int secs = static_cast<int>(seconds) % 60;

    std::ostringstream oss;
    if (hours > 0) {
      oss << hours << "h ";
    }
    if (hours > 0 || minutes > 0) {
      oss << minutes << "m ";
    }
    oss << secs << "s";
    return oss.str();
  }

  std::string FormatSize(size_t bytes) const {
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    double size = bytes;

    while (size >= 1024 && unit < 4) {
      size /= 1024;
      unit++;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << size << units[unit];
    return oss.str();
  }

  void PrintBar(double percentage) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::seconds>(now - start_time_)
            .count();

    // Calculate speed and ETA
    double speed = elapsed > 0 ? (current_ / elapsed) : 0;
    double eta = speed > 0 ? (total_ - current_) / speed : 0;

    // Clear the line and move to start
    std::cout << "\r";

    // Print rank and title
    std::cout << "Rank " << std::setw(3) << rank_ << " [" << std::setw(20)
              << std::left << title_ << "] ";

    // Print progress bar
    int pos = static_cast<int>(width_ * percentage / 100.0);
    std::cout << "[";
    for (int i = 0; i < width_; ++i) {
      if (i < pos)
        std::cout << "=";
      else if (i == pos)
        std::cout << ">";
      else
        std::cout << " ";
    }
    std::cout << "] ";

    // Print percentage and progress details
    std::cout << std::fixed << std::setprecision(1) << std::setw(5)
              << percentage << "% " << "(" << FormatSize(current_) << "/"
              << FormatSize(total_) << ") " << FormatSize(speed) << "/s ";

    if (percentage < 100) {
      std::cout << "ETA: " << FormatTime(eta);
    } else {
      std::cout << "Time: " << FormatTime(elapsed);
    }

    // Fill rest of line with spaces and flush
    std::cout << std::string(10, ' ') << std::flush;
  }

  std::string title_;
  size_t total_;
  size_t current_;
  int width_;
  int rank_;
  std::chrono::steady_clock::time_point start_time_;
};

} // namespace cae

#endif // CAE_FORMAT_PROGRESS_BAR_H_