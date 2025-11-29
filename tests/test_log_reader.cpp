#include <gtest/gtest.h>
#include "../src/log_reader.hpp"
#include <fstream>
#include <filesystem>

class LogReaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary test log file
        test_file_ = "test_log.txt";
        std::ofstream ofs(test_file_);
        ofs << "Line 1: This is the first line\n";
        ofs << "Line 2: This is the second line\n";
        ofs << "Line 3: ERROR something went wrong\n";
        ofs << "Line 4: INFO everything is fine\n";
        ofs << "Line 5: DEBUG detailed information\n";
        ofs.close();
    }

    void TearDown() override {
        // Clean up test file
        if (std::filesystem::exists(test_file_)) {
            std::filesystem::remove(test_file_);
        }
    }

    std::string test_file_;
};

TEST_F(LogReaderTest, OpenValidFile) {
    LogReader reader;
    EXPECT_TRUE(reader.open(test_file_));
    EXPECT_TRUE(reader.isOpen());
    EXPECT_EQ(reader.getFilename(), test_file_);
}

TEST_F(LogReaderTest, OpenInvalidFile) {
    LogReader reader;
    EXPECT_FALSE(reader.open("non_existent_file.log"));
    EXPECT_FALSE(reader.isOpen());
}

TEST_F(LogReaderTest, GetLineCount) {
    LogReader reader;
    ASSERT_TRUE(reader.open(test_file_));
    EXPECT_EQ(reader.getLineCount(), 5);
}

TEST_F(LogReaderTest, GetLine) {
    LogReader reader;
    ASSERT_TRUE(reader.open(test_file_));

    auto line0 = reader.getLine(0);
    EXPECT_EQ(line0, "Line 1: This is the first line");

    auto line2 = reader.getLine(2);
    EXPECT_EQ(line2, "Line 3: ERROR something went wrong");
}

TEST_F(LogReaderTest, GetLines) {
    LogReader reader;
    ASSERT_TRUE(reader.open(test_file_));

    auto lines = reader.getLines(1, 3);
    EXPECT_EQ(lines.size(), 3);
    EXPECT_EQ(lines[0], "Line 2: This is the second line");
    EXPECT_EQ(lines[1], "Line 3: ERROR something went wrong");
    EXPECT_EQ(lines[2], "Line 4: INFO everything is fine");
}

TEST_F(LogReaderTest, GetFileSize) {
    LogReader reader;
    ASSERT_TRUE(reader.open(test_file_));

    size_t expected_size = std::filesystem::file_size(test_file_);
    EXPECT_EQ(reader.getFileSize(), expected_size);
}

TEST_F(LogReaderTest, CloseFile) {
    LogReader reader;
    ASSERT_TRUE(reader.open(test_file_));
    EXPECT_TRUE(reader.isOpen());

    reader.close();
    EXPECT_FALSE(reader.isOpen());
}

TEST_F(LogReaderTest, EmptyFile) {
    std::string empty_file = "empty.txt";
    std::ofstream ofs(empty_file);
    ofs.close();

    LogReader reader;
    EXPECT_TRUE(reader.open(empty_file));
    EXPECT_EQ(reader.getLineCount(), 0);

    std::filesystem::remove(empty_file);
}
