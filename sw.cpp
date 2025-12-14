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

enum class task {
    run_nameless,
    run_named,  // Possibly create a new named timer, possibly load existing from config
    list_timers,  // List all the named timers in the config file and exit
    delete_named,  // Delete a named timer from the config file and exit
};

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

void run(std::chrono::system_clock::duration elapsed_saved_timer,
         std::chrono::steady_clock::time_point program_start_stdy) {
    std::string dest;
    std::back_insert_iterator out{dest};
    while (true) {
        const chrono::steady_clock::time_point tnow = chrono::steady_clock::now();
        const std::chrono::duration elapsed = elapsed_saved_timer + tnow - program_start_stdy;

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
}

int main(int argc, char* argv[]) {
    // Delay between calculating these two quantities introduces a (hopefullly) tiny amount of error when
    // loading a named timer.
    std::chrono::system_clock::time_point program_start_sys = std::chrono::system_clock::now();
    std::chrono::steady_clock::time_point program_start_stdy = std::chrono::steady_clock::now();

    task task = determine_task(argc, argv);

    // sw uses the steady_clock for measuring elapsed time while running, but the system_clock for saving
    // named timers.  elapsed_saved_timer allows us to combine these two incommensurate clocks by converting
    // the saved start time (for a saved, named timer) to a duration which can be added to a steady_clock
    // time_point.
    std::chrono::duration<std::chrono::system_clock::rep, std::chrono::system_clock::period> elapsed_saved_timer {0};
    std::optional<std::filesystem::path> config_file_path = get_config_file_path();
    std::vector<timer_entry> saved_timers;
    if (config_file_path) {
        saved_timers = read_config_file(*config_file_path);

        // When did the timer start?  It's either the value in the config file, or when the program was started
        std::optional<std::chrono::system_clock::time_point> named_timer_start {};
        if (task==task::run_named) {
            named_timer_start = tstart_if_exists(saved_timers, argv[1]);
        }

        if (task==task::run_named && !named_timer_start) {
            // The user passed in a timer name but no existing timer was found.
            // Create a new one.
            timer_entry new_timer{.timer_name=argv[1], .start_time=program_start_sys};
            write_to_config_file(*config_file_path, new_timer);
        }
        
        if (named_timer_start) {
            elapsed_saved_timer = program_start_sys - *named_timer_start;
        }
    }

    if (task == task::run_nameless || task == task::run_named) {
        run(elapsed_saved_timer, program_start_stdy);
    } else if (task == task::list_timers) {
        for (const auto& timer : saved_timers) {
            std::printf("%s\n",timer.timer_name.c_str());
        }
    }
    
    return 0;
}

