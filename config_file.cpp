#include "config.h"

#include <charconv>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include <fstream>


std::vector<timer_entry> read_config_file(const std::filesystem::path& path) {
    std::vector<timer_entry> entries;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        return {};
    }
    
    std::string line;
    std::string current_timer_name;
    
    while (std::getline(file, line)) {
        // Trim leading/trailing whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        size_t end = line.find_last_not_of(" \t\r\n");
        
        if (start == std::string::npos) {
            continue; // Empty line, skip it
        }
        
        // check end > start?
        std::string_view line_payload {line.data()+start, end - start + 1};
        
        // Check if line is a timer name (enclosed in square brackets)
        if (line_payload.front() == '[' && line_payload.back() == ']') {
            // Extract the timer name
            current_timer_name = std::string(line_payload.substr(1, line_payload.size() - 2));
        } else if (!current_timer_name.empty()) {
            // Extract timestamp for the current timer
            uint64_t timestamp {0};
            [[maybe_unused]] std::from_chars_result result = std::from_chars(
                line_payload.data(),
                line_payload.data() + line_payload.size(),
                timestamp
            );
            auto time_point = std::chrono::system_clock::time_point(
                std::chrono::milliseconds(timestamp)
            );
            entries.push_back(timer_entry{.timer_name=std::move(current_timer_name), .start_time=time_point});
            current_timer_name.clear();
        }
    }
    
    return entries;
}   








