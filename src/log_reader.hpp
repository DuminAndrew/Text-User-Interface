#pragma once

#include <string>
#include <vector>
#include <string_view>
#include <memory>
#include <cstddef>
#include <fstream>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX  // Предотвращает определение макросов min/max
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

class LogReader {
public:
    LogReader();
    ~LogReader();

    // Open file using mmap for small files or buffered reading for large files
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
    bool isOpen() const;

    // Get filename
    const std::string& getFilename() const { return filename_; }

private:
    void indexLines();
    void indexLinesLargeFile();
    std::string readLineFromFile(size_t offset, size_t length) const;

    static constexpr size_t MAX_MMAP_SIZE = 2ULL * 1024 * 1024 * 1024; // 2GB limit

    std::string filename_;
    char* mapped_data_;
    size_t file_size_;
    std::vector<size_t> line_offsets_;  // Offset of each line start
    bool use_mmap_;  // true for small files, false for large files

    // For large file support
    mutable std::unique_ptr<std::ifstream> file_stream_;
    mutable std::vector<std::string> line_cache_;  // Cache for large files

#ifdef _WIN32
    HANDLE file_handle_;
    HANDLE mapping_handle_;
#else
    int fd_;
#endif
};
