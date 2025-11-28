#include "config.h"

#include <format>
#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <cstdio>

namespace chrono {
    using namespace std::chrono;
}

struct decomposed_duration {
    chrono::days days;
    chrono::hours hr;
    chrono::minutes min;
    chrono::seconds sec;
    chrono::milliseconds ms;
};

template<typename Rep, typename Period>
decomposed_duration decompose_duration(std::chrono::duration<Rep,Period> dur) {
    const auto d = chrono::duration_cast<chrono::days>(dur);
    const auto hrs = chrono::duration_cast<chrono::hours>(dur) - d;
    const auto min = chrono::duration_cast<chrono::minutes>(dur) - hrs - d;
    const auto sec = chrono::duration_cast<chrono::seconds>(dur) - min - hrs - d;
    const auto ms = chrono::duration_cast<chrono::milliseconds>(dur) - sec - min - hrs - d;

    return decomposed_duration{.days=d, .hr=hrs, .min=min, .sec=sec, .ms=ms};
}


int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    std::chrono::steady_clock::time_point tstart_stdy = std::chrono::steady_clock::now();

    std::chrono::duration<std::chrono::system_clock::rep, std::chrono::system_clock::period> elapsed_base {0};
    std::optional<std::filesystem::path> config_file_path = get_config_path();
    if (config_file_path) {
        std::vector<timer_entry> saved_timers = read_config_file(*config_file_path);

        std::optional<std::chrono::system_clock::time_point> tstart {};
        if (argc == 2) {
            tstart = tstart_if_exists(saved_timers, argv[1]);
        }

        if (argc == 2 && !tstart) {
            // The user passed in a timer name (argc == 2) but no existing timer was found.
            // Create a new one.
            // TODO:  This tstart needs to move to the top of the function; call it program_start or something
            tstart = std::chrono::system_clock::now();
            timer_entry new_timer{.timer_name=argv[1], .start_time=*tstart};
            write_to_config_file(*config_file_path, new_timer);
        }
        
        if (tstart) {
            elapsed_base = std::chrono::system_clock::now() - *tstart;
            tstart_stdy = std::chrono::steady_clock::now();
        }
    }

    std::string dest;
    std::back_insert_iterator out{dest};
    while (true) {
        const chrono::steady_clock::time_point tnow = chrono::steady_clock::now();
        const std::chrono::duration elapsed = elapsed_base + tnow - tstart_stdy;

        const auto [d, hrs, min, sec, ms] = decompose_duration(elapsed);

        if (d.count() > 0) {
            std::format_to(out, "  {}:{:02}:{:02}:{:02}:{:03}          \r",
                d.count(),hrs.count(),min.count(),sec.count(),ms.count());
        } else if (hrs.count() > 0) {
            std::format_to(out, "  {}:{:02}:{:02}:{:03}             \r",
                hrs.count(),min.count(),sec.count(),ms.count());
        } else if (min.count() > 0) {
            std::format_to(out, "  {}:{:02}:{:03}                \r",
                min.count(),sec.count(),ms.count());
        } else if (sec.count() > 0) {
            std::format_to(out, "  {}:{:03}                   \r",
                sec.count(),ms.count());
        } else {
            std::format_to(out, "  {}                      \r",
                ms.count());
        }

        //std::cout << dest << std::flush;
        std::printf(dest.c_str());
        std::fflush(stdout);
        dest.clear();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }


    return 0;
}

