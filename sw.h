#pragma once

#include <chrono>
#include <filesystem>
#include <optional>
#include <chrono>
#include <string>
#include <vector>
#include <span>


enum class task {
    run_nameless,
    run_named,  // Possibly create a new named timer, possibly load existing from config
    list_timers,  // List all the named timers in the config file and exit
    delete_named,  // Delete a named timer from the config file and exit
};
task determine_task(int argc, char* argv[]);


//
// Generic file reading and writing utilities.  Read and write an entire file in one-shot.
//
std::optional<std::vector<char>> read_file(const std::filesystem::path& path);
bool write_file(const std::filesystem::path& path, std::span<const char> file_data);


struct timer_entry{
    std::string timer_name;
    std::chrono::system_clock::time_point start_time;
};
std::vector<timer_entry> decode_config_file_data(std::span<const char> file_data);
std::vector<char> encode_config_file_data(std::span<const timer_entry> entries);
std::optional<std::filesystem::path> get_config_file_path();


std::optional<std::chrono::system_clock::time_point> tstart_if_exists(std::span<timer_entry>, std::string_view);

