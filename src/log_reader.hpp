#pragma once

#include <string>
#include <vector>
#include <string_view>
#include <memory>
#include <cstddef>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

class LogReader {
public:
    LogReader();
    ~LogReader();

    // Open file using mmap
    bool open(const std::string& filename);

    // Close file and unmap memory
    void close();

    // Get total number of lines
    size_t getLineCount() const { return line_offsets_.size(); }

    // Get line by index (zero-based)
    std::string_view getLine(size_t index) const;

    // Get range of lines
    std::vector<std::string_view> getLines(size_t start, size_t count) const;

    // Get file size
    size_t getFileSize() const { return file_size_; }

    // Check if file is opened
    bool isOpen() const { return mapped_data_ != nullptr; }

    // Get filename
    const std::string& getFilename() const { return filename_; }

private:
    void indexLines();

    std::string filename_;
    int fd_;
    char* mapped_data_;
    size_t file_size_;
    std::vector<size_t> line_offsets_;  // Offset of each line start
};
