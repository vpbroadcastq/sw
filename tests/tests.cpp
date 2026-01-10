#include "testdata.h"

#include <array>
#include <gtest/gtest.h>
#include <swlib/sw.h>
#include <vector>

//
// determine_task tests
//
TEST(determine_task, RunNameless) {
    const char* argv[] = {"sw"};
    sw::task result = sw::determine_task(1, const_cast<char**>(argv));
    EXPECT_EQ(result, sw::task::run_nameless);
}

TEST(determine_task, ListTimers) {
    {
        const char* argv[] = {"sw", "-l"};
        sw::task result = sw::determine_task(2, const_cast<char**>(argv));
        EXPECT_EQ(result, sw::task::list_timers);
    }
    {
        const char* argv[] = {"sw", "--list-timers"};
        sw::task result = sw::determine_task(2, const_cast<char**>(argv));
        EXPECT_EQ(result, sw::task::list_timers);
    }
}

TEST(determine_task, PrintHelp) {
    {
        const char* argv[] = {"sw", "-h"};
        sw::task result = sw::determine_task(2, const_cast<char**>(argv));
        EXPECT_EQ(result, sw::task::print_help);
    }
    {
        const char* argv[] = {"sw", "-?"};
        sw::task result = sw::determine_task(2, const_cast<char**>(argv));
        EXPECT_EQ(result, sw::task::print_help);
    }
    {
        const char* argv[] = {"sw", "--help"};
        sw::task result = sw::determine_task(2, const_cast<char**>(argv));
        EXPECT_EQ(result, sw::task::print_help);
    }
}

TEST(determine_task, RunNamed) {
    {
        const char* argv[] = {"sw", "mytimer"};
        sw::task result = sw::determine_task(2, const_cast<char**>(argv));
        EXPECT_EQ(result, sw::task::run_named);
    }
    {
        const char* argv[] = {"sw", "123"};
        sw::task result = sw::determine_task(2, const_cast<char**>(argv));
        EXPECT_EQ(result, sw::task::run_named);
    }
    {
        const char* argv[] = {"sw", "my_timer_name"};
        sw::task result = sw::determine_task(2, const_cast<char**>(argv));
        EXPECT_EQ(result, sw::task::run_named);
    }
}

TEST(determine_task, InvalidCommandline) {
    // Made up flags/timer name can't begin with a -
    {
        const char* argv[] = {"sw", "-x"};
        sw::task result = sw::determine_task(2, const_cast<char**>(argv));
        EXPECT_EQ(result, sw::task::invalid_commandline);
    }
    {
        const char* argv[] = {"sw", "--unknown-flag"};
        sw::task result = sw::determine_task(2, const_cast<char**>(argv));
        EXPECT_EQ(result, sw::task::invalid_commandline);
    }
    // Missing timer name to delete
    {
        const char* argv[] = {"sw", "-d"};
        sw::task result = sw::determine_task(2, const_cast<char**>(argv));
        EXPECT_EQ(result, sw::task::invalid_commandline);
    }
    {
        const char* argv[] = {"sw", "--delete-timer"};
        sw::task result = sw::determine_task(2, const_cast<char**>(argv));
        EXPECT_EQ(result, sw::task::invalid_commandline);
    }
    // You can't pass in what looks like multiple names
    {
        const char* argv[] = {"sw", "arg1", "arg2"};
        sw::task result = sw::determine_task(3, const_cast<char**>(argv));
        EXPECT_EQ(result, sw::task::invalid_commandline);
    }
    {
        const char* argv[] = {"sw", "arg1", "arg2", "arg3"};
        sw::task result = sw::determine_task(4, const_cast<char**>(argv));
        EXPECT_EQ(result, sw::task::invalid_commandline);
    }
}

TEST(determine_task, DeleteNamed) {
    {
        const char* argv[] = {"sw", "-d", "mytimer"};
        sw::task result = sw::determine_task(3, const_cast<char**>(argv));
        EXPECT_EQ(result, sw::task::delete_named);
    }
    {
        const char* argv[] = {"sw", "--delete-timer", "mytimer"};
        sw::task result = sw::determine_task(3, const_cast<char**>(argv));
        EXPECT_EQ(result, sw::task::delete_named);
    }
}


//
// is_valid_timer_name tests
//
TEST(is_valid_timer_name, ValidExamples) {
    for (const auto& name : testdata::valid_timer_names) {
        EXPECT_TRUE(sw::is_valid_timer_name(name)) << "Expected valid timer name: \"" << name << "\"";
    }
}

TEST(is_valid_timer_name, InvalidExamples) {
    for (const auto& name : testdata::invalid_timer_names) {
        EXPECT_FALSE(sw::is_valid_timer_name(name)) << "Expected invalid timer name: \"" << name << "\"";
    }
}


//
// decode_config_file_data tests
//
TEST(decode_config_file_data, NoData) {
    std::array<char,0> emptyArray;
    std::vector<sw::timer_entry> entries = sw::decode_config_file_data(emptyArray);
    EXPECT_TRUE(entries.empty());
}
TEST(decode_config_file_data, InvalidExamples) {
    std::vector<sw::timer_entry> entries = sw::decode_config_file_data(testdata::all_invalid);
    EXPECT_TRUE(entries.empty());
}
TEST(decode_config_file_data, ValidExamples) {
    std::vector<sw::timer_entry> entries = sw::decode_config_file_data(testdata::all_valid);
    EXPECT_EQ(entries.size(), 3);

    ASSERT_EQ(entries.size(), testdata::all_valid_entries.size());
    for (size_t i=0; i<entries.size(); ++i) {
        EXPECT_EQ(entries[i].timer_name, testdata::all_valid_entries[i].timer_name);
        EXPECT_EQ(entries[i].start_time, testdata::all_valid_entries[i].start_time);
    }
}
TEST(decode_config_file_data, MixedValidInvalidExamples) {
    std::vector<sw::timer_entry> entries = sw::decode_config_file_data(testdata::mixed_valid_invalid);
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
    std::array<sw::timer_entry,0> emptyArray;
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


