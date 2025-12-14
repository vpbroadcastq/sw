#include <array>
#include <filesystem>
#include <gtest/gtest.h>
#include "../config.h"
#include <string_view>
#include <vector>



TEST(read_config_file, FileThatDoesNotExist) {
    std::filesystem::path p("./data/ThisFileDoesNotExist.ini");
    ASSERT_FALSE(std::filesystem::exists(p));

    std::vector<timer_entry> entries = read_config_file(p);
    EXPECT_TRUE(entries.empty());
}

TEST(read_config_file, InvalidExamples) {
    std::filesystem::path p("./data/bad.ini");
    ASSERT_TRUE(std::filesystem::exists(p));

    std::vector<timer_entry> entries = read_config_file(p);
    EXPECT_TRUE(entries.empty());
}

TEST(read_config_file, EmptyConfigFile) {
    std::filesystem::path p("./data/empty.ini");
    ASSERT_TRUE(std::filesystem::exists(p));

    std::vector<timer_entry> entries = read_config_file(p);
    EXPECT_TRUE(entries.empty());
}


// TODO:  Test the value of the timer
TEST(read_config_file, ValidExamples) {
    std::filesystem::path p("./data/good.ini");
    ASSERT_TRUE(std::filesystem::exists(p));

    static constexpr std::array<std::string_view,3> expected_names {
        "whatever",
        "name with spaces",
        "Name~with \" special' ;chars!@#$%^&"
    };

    std::vector<timer_entry> entries = read_config_file(p);
    ASSERT_EQ(entries.size(), expected_names.size());
    for (size_t i; i<entries.size(); ++i) {
        EXPECT_EQ(entries[i].timer_name, expected_names[i]);
    }
}