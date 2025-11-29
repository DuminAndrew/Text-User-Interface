#include "log_reader.hpp"
#include <iostream>
#include <cstring>

LogReader::LogReader()
    : fd_(-1)
    , mapped_data_(nullptr)
    , file_size_(0) {
}

LogReader::~LogReader() {
    close();
}

bool LogReader::open(const std::string& filename) {
    close();  // Close any previously opened file

    filename_ = filename;

    // Open file
    fd_ = ::open(filename.c_str(), O_RDONLY);
    if (fd_ == -1) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    // Get file size
    struct stat sb;
    if (fstat(fd_, &sb) == -1) {
        std::cerr << "Failed to get file size" << std::endl;
        ::close(fd_);
        fd_ = -1;
        return false;
    }

    file_size_ = sb.st_size;

    // Handle empty files
    if (file_size_ == 0) {
        ::close(fd_);
        fd_ = -1;
        mapped_data_ = nullptr;
        return true;
    }

    // Map file into memory
    mapped_data_ = static_cast<char*>(
        mmap(nullptr, file_size_, PROT_READ, MAP_PRIVATE, fd_, 0)
    );

    if (mapped_data_ == MAP_FAILED) {
        std::cerr << "Failed to mmap file" << std::endl;
        ::close(fd_);
        fd_ = -1;
        mapped_data_ = nullptr;
        return false;
    }

    // Advise kernel about access pattern
    madvise(mapped_data_, file_size_, MADV_SEQUENTIAL);

    // Index all lines
    indexLines();

    return true;
}

void LogReader::close() {
    if (mapped_data_ != nullptr && mapped_data_ != MAP_FAILED) {
        munmap(mapped_data_, file_size_);
        mapped_data_ = nullptr;
    }

    if (fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }

    file_size_ = 0;
    line_offsets_.clear();
    filename_.clear();
}

void LogReader::indexLines() {
    line_offsets_.clear();

    if (file_size_ == 0) {
        return;
    }

    // Reserve space for efficiency (estimate ~80 bytes per line)
    line_offsets_.reserve(file_size_ / 80);

    // First line always starts at 0
    line_offsets_.push_back(0);

    // Scan for newlines
    for (size_t i = 0; i < file_size_; ++i) {
        if (mapped_data_[i] == '\n') {
            if (i + 1 < file_size_) {
                line_offsets_.push_back(i + 1);
            }
        }
    }
}

std::string_view LogReader::getLine(size_t index) const {
    if (index >= line_offsets_.size() || mapped_data_ == nullptr) {
        return std::string_view();
    }

    size_t start = line_offsets_[index];
    size_t end;

    if (index + 1 < line_offsets_.size()) {
        end = line_offsets_[index + 1] - 1;  // -1 to exclude the newline
    } else {
        end = file_size_;
    }

    // Handle trailing newline
    if (end > start && mapped_data_[end - 1] == '\n') {
        end--;
    }

    size_t length = end - start;
    return std::string_view(mapped_data_ + start, length);
}

std::vector<std::string_view> LogReader::getLines(size_t start, size_t count) const {
    std::vector<std::string_view> result;
    result.reserve(count);

    for (size_t i = start; i < start + count && i < line_offsets_.size(); ++i) {
        result.push_back(getLine(i));
    }

    return result;
}
