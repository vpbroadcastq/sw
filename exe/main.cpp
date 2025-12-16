#include <swlib/sw.h>

#include <format>
#include <string>
#include <chrono>
#include <thread>
#include <cstdio>
#include <ranges>
#include <algorithm>


struct decomposed_duration {
    std::chrono::days days;
    std::chrono::hours hr;
    std::chrono::minutes min;
    std::chrono::seconds sec;
    std::chrono::milliseconds ms;
};

template<typename Rep, typename Period>
decomposed_duration decompose_duration(std::chrono::duration<Rep,Period> dur) {
    const auto d = std::chrono::duration_cast<std::chrono::days>(dur);
    const auto hrs = std::chrono::duration_cast<std::chrono::hours>(dur) - d;
    const auto min = std::chrono::duration_cast<std::chrono::minutes>(dur) - hrs - d;
    const auto sec = std::chrono::duration_cast<std::chrono::seconds>(dur) - min - hrs - d;
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur) - sec - min - hrs - d;
    return decomposed_duration{.days=d, .hr=hrs, .min=min, .sec=sec, .ms=ms};
}

void run(std::chrono::system_clock::duration elapsed_saved_timer,
         std::chrono::steady_clock::time_point program_start_stdy) {
    std::string dest;
    std::back_insert_iterator out{dest};
    while (true) {
        const std::chrono::steady_clock::time_point tnow = std::chrono::steady_clock::now();
        const std::chrono::duration elapsed = elapsed_saved_timer + (tnow - program_start_stdy);

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

    sw::task task = sw::determine_task(argc, argv);

    // sw uses the steady_clock for measuring elapsed time while running, but the system_clock for saving
    // named timers.  elapsed_saved_timer allows us to combine these two incommensurate clocks by converting
    // the saved start time (for a saved, named timer) to a duration which can be added to a steady_clock
    // time_point.
    std::chrono::duration<std::chrono::system_clock::rep, std::chrono::system_clock::period> elapsed_saved_timer {0};
    std::optional<std::filesystem::path> config_file_path = sw::get_config_file_path();
    std::vector<sw::timer_entry> saved_timers;
    if (config_file_path) {
        {
            std::optional<std::vector<char>>  config_file_data = sw::read_file(*config_file_path);
            if (config_file_data) {
                saved_timers = sw::decode_config_file_data(*config_file_data);
            }
        }

        if (task == sw::task::delete_named) {
            auto subr = std::ranges::remove_if(saved_timers,
                [name=std::string_view{argv[1]}](const sw::timer_entry& entry)-> bool {
                    return entry.timer_name == name;
                });
            if (!subr.empty()) {
                saved_timers.erase(subr.begin(), subr.end());
                std::vector<char> new_config_file_data = sw::encode_config_file_data(saved_timers);
                sw::write_file(*config_file_path, new_config_file_data);
            } else {
                std::fprintf(stderr, "No timer named \"%s\" found in config file.\n", argv[1]);
                return 1;
            }
            return 0;
        } else if (task == sw::task::list_timers) {
            for (const auto& timer : saved_timers) {
                std::printf("%s\n",timer.timer_name.c_str());
            }
            return 0;
        }

        // When did the timer start?  It's either the value in the config file, or when the program was started
        std::optional<std::chrono::system_clock::time_point> named_timer_start {};
        if (task==sw::task::run_named) {
            named_timer_start = sw::tstart_if_exists(saved_timers, argv[1]);
        }

        if (task==sw::task::run_named && !named_timer_start) {
            // The user passed in a timer name but no existing timer was found.  Create a new one.
            sw::timer_entry new_timer{.timer_name=argv[1], .start_time=program_start_sys};
            saved_timers.push_back(new_timer);
            std::vector<char> new_config_file_data = sw::encode_config_file_data(saved_timers);
            sw::write_file(*config_file_path, new_config_file_data);
        }
        
        if (named_timer_start) {
            elapsed_saved_timer = program_start_sys - *named_timer_start;
        }
    } else {
        // Unable to get a config file path.  This is only an error if the task requires a config file.
        if (task == sw::task::run_named || task == sw::task::list_timers || task == sw::task::delete_named) {
            std::fprintf(stderr, "Unable to determine config file path.\n");
            return 1;
        }
    }

    if (task == sw::task::run_nameless || task == sw::task::run_named) {
        run(elapsed_saved_timer, program_start_stdy);
    } 
    
    return 0;
}

