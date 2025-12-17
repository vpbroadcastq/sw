#include "sw.h"

#include <charconv>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include <algorithm>
#include <ranges>

namespace sw {
//
// Generic file reading utility.  Reads an entire file in one-shot.
//
std::optional<std::vector<char>> read_file(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
        return std::nullopt;
    }
    std::error_code ec {};
    std::uintmax_t nbytes = std::filesystem::file_size(path, ec);
    if (ec) {
        return std::nullopt;
    }

    std::FILE* f = std::fopen(path.string().c_str(), "rb");
    if (!f) {
        return std::nullopt;
    }

    std::vector<char> file_data(nbytes);
    size_t nread = std::fread(file_data.data(), 1, nbytes, f);
    std::fclose(f);
    if (nread != nbytes) {
        return std::nullopt;
    }
    return file_data;
}

//
// Generic file writing utility.  Overwrites existing file, or creates a new one if needed.
//
bool write_file(const std::filesystem::path& path, std::span<const char> file_data) {
    std::FILE* f = std::fopen(path.string().c_str(), "wb");
    if (!f) {
        return false;
    }

    size_t nwrite = std::fwrite(file_data.data(), 1, file_data.size(), f);
    std::fclose(f);
    if (nwrite != file_data.size()) {
        return false;
    }
    return true;
}


//
// Parses config file data from a span of characters into timer_entry structs.  Any invalid entries are skipped,
// so that writing the vector<timer_entry> back to file will clean up any malformed data.
//
std::vector<timer_entry> decode_config_file_data(std::span<const char> file_data) {
    auto is_whitespace = [](const char& c)->bool
        { return c == ' ' || c == '\t' || c == '\r'; };

    std::vector<timer_entry> entries;
    std::string current_timer_name;
    std::ranges::split_view file_lines {file_data, '\n' };
    for (const auto& line : file_lines) {
        // Extract a view into line (line_payload) that does not include leading and trailing whitespace
        auto beg = std::ranges::find_if_not(line, is_whitespace);
        auto end = std::ranges::find_if_not(line | std::views::reverse, is_whitespace).base();
        if (beg == end) {
            continue;
        }
        std::string_view line_payload(&*beg, static_cast<std::size_t>(std::ranges::distance(beg, end)));

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
                current_timer_name.clear();
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

std::vector<char> encode_config_file_data(std::span<const timer_entry> entries) {
    std::vector<char> file_data;
    std::back_insert_iterator out{file_data};
    for (const auto& entry : entries) {
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            entry.start_time.time_since_epoch()
        ).count();
        std::format_to(out, "[{}]\n{}\n\n", entry.timer_name, timestamp);
    }

    return file_data;
}


// Searches for a timer entry with the given name and returns its start time if found
std::optional<std::chrono::system_clock::time_point> tstart_if_exists(std::span<const timer_entry> entries, std::string_view name) {
    auto it = std::ranges::find_if(entries, 
        [name](const timer_entry& entry){
            return entry.timer_name == name;
    });
    if (it != entries.end()) {
        return it->start_time;
    }
    return std::nullopt;
}

// TODO:  This returns run_nameless in cases where the command line is invalid.  Warn the user instead?
//        Specifically, as one example proble, an invalid flag like -x is going to be interpreted as a
//        timer name.
task determine_task(int argc, char* argv[]) {
    if (argc == 1) {
        return task::run_nameless;
    }

    if (argc == 2) {
        std::string_view arg1{argv[1]};
        if (arg1 == "-l" || arg1 == "--list-timers") {
            return task::list_timers;
        } else {
            return task::run_named;
        }
    }

    if (argc == 3) {
        std::string_view arg1{argv[1]};
        if (arg1 == "-d" || arg1 == "--delete-timer") {
            return task::delete_named;
        }
    }

    return task::run_nameless;
}

} // namespace sw
