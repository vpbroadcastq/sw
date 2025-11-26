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

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    [[maybe_unused]] const std::chrono::system_clock::time_point tstart_sys = std::chrono::system_clock::now();
    const std::chrono::steady_clock::time_point tstart_stdy = std::chrono::steady_clock::now();

    auto p = get_config_path();
    std::cout << *p << std::endl;

    std::string dest;
    std::back_insert_iterator out{dest};

    while (true) {
        const chrono::steady_clock::time_point tnow = chrono::steady_clock::now();
        const std::chrono::duration elapsed = tnow - tstart_stdy;

        const auto d = chrono::duration_cast<chrono::days>(elapsed);
        const auto hrs = chrono::duration_cast<chrono::hours>(elapsed) - d;
        const auto min = chrono::duration_cast<chrono::minutes>(elapsed) - hrs - d;
        const auto sec = chrono::duration_cast<chrono::seconds>(elapsed) - min - hrs - d;
        const auto ms = chrono::duration_cast<chrono::milliseconds>(elapsed) - sec - min - hrs - d;

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

