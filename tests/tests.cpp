#include "testdata.h"

#include <array>
#include <gtest/gtest.h>
#include "../sw.h"
#include <string_view>
#include <vector>

//
// decode_config_file_data tests
//
TEST(decode_config_file_data, NoData) {
    std::array<char,0> emptyArray;
    std::vector<timer_entry> entries = decode_config_file_data(emptyArray);
    EXPECT_TRUE(entries.empty());
}
TEST(decode_config_file_data, InvalidExamples) {
    std::vector<timer_entry> entries = decode_config_file_data(testdata::all_invalid);
    EXPECT_TRUE(entries.empty());
}
TEST(decode_config_file_data, ValidExamples) {
    std::vector<timer_entry> entries = decode_config_file_data(testdata::all_valid);
    EXPECT_EQ(entries.size(), 3);

    ASSERT_EQ(entries.size(), testdata::all_valid_entries.size());
    for (size_t i=0; i<entries.size(); ++i) {
        EXPECT_EQ(entries[i].timer_name, testdata::all_valid_entries[i].timer_name);
        EXPECT_EQ(entries[i].start_time, testdata::all_valid_entries[i].start_time);
    }
}
TEST(decode_config_file_data, MixedValidInvalidExamples) {
    std::vector<timer_entry> entries = decode_config_file_data(testdata::mixed_valid_invalid);
    EXPECT_EQ(entries.size(), 4);

    ASSERT_EQ(entries.size(), testdata::mixed_valid_invalid_entries.size());
    for (size_t i=0; i<entries.size(); ++i) {
        EXPECT_EQ(entries[i].timer_name, testdata::mixed_valid_invalid_entries[i].timer_name);
        EXPECT_EQ(entries[i].start_time, testdata::mixed_valid_invalid_entries[i].start_time);
    }
}

//
// encode_config_file_data tests
//
TEST(encode_config_file_data, EmptyEntries) {
    std::array<timer_entry,0> emptyArray;
    std::vector<char> file_data = encode_config_file_data(emptyArray);
    EXPECT_TRUE(file_data.empty());
}
TEST(encode_config_file_data, ValidEntries) {
    std::vector<char> file_data = encode_config_file_data(testdata::all_valid_entries);
    std::string_view file_data_view{file_data.data(), file_data.size()};
    EXPECT_EQ(file_data_view, testdata::all_valid_reencoded);
}
TEST(encode_config_file_data, MixedValidInvalid) {
    std::vector<char> file_data = encode_config_file_data(testdata::mixed_valid_invalid_entries);
    std::string_view file_data_view{file_data.data(), file_data.size()};
    EXPECT_EQ(file_data_view, testdata::mixed_valid_invalid_reencoded);
}


