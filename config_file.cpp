#include "config.h"

#include <charconv>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include <fstream>
#include <ranges>

// TODO:  More robust error handling and reporting
// TODO:  Don't use iostreams/getline.  Just slurp in the whole file at once
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
            std::from_chars_result result = std::from_chars(
                line_payload.data(),
                line_payload.data() + line_payload.size(),
                timestamp
            );
            if (result.ec != std::errc{}) {
                continue; // Failed to parse timestamp, skip this line
            }
            auto time_point = std::chrono::system_clock::time_point(
                std::chrono::milliseconds(timestamp)
            );
            entries.push_back(timer_entry{.timer_name=std::move(current_timer_name), .start_time=time_point});
            current_timer_name.clear();
        }
    }
    
    return entries;
}


// TODO:  Don't use iostreams
// Does not check for existing entries with the same name     
bool write_to_config_file(const std::filesystem::path& file_path, const timer_entry& timer) {
    std::ofstream file(file_path, std::ios::app);
    if (!file.is_open()) {
        return false;
    }
    
    file << "\n[" << timer.timer_name << "]\n";
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        timer.start_time.time_since_epoch()
    ).count();
    file << timestamp << "\n";
    
    return true;
}


// Searches for a timer entry with the given name and returns its start time if found
std::optional<std::chrono::system_clock::time_point> tstart_if_exists(std::span<timer_entry> entries, std::string_view name) {
    auto it = std::ranges::find_if(entries, 
        [name](const timer_entry& entry){
            return entry.timer_name == name;
    });
    if (it != entries.end()) {
        return it->start_time;
    }
    return std::nullopt;
}






