#pragma once

#include <chrono>
#include <filesystem>
#include <optional>
#include <chrono>
#include <string>
#include <vector>



std::optional<std::filesystem::path> get_config_path();


struct timer_entry{
    std::string timer_name;
    std::chrono::system_clock::time_point start_time;
};
std::vector<timer_entry> read_config_file(const std::filesystem::path& path);



