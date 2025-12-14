#include "config.h"

#include <filesystem>
#include <cstdlib>
#include <optional>


// TODO:  Windows & mac testing
std::optional<std::filesystem::path> get_config_file_path() {
    std::filesystem::path config_file_name{"sw.ini"};

    const char* xdg_path = std::getenv("XDG_CONFIG_HOME");
    if (xdg_path) {
        return std::filesystem::path(xdg_path) / config_file_name;
    }

    const char* home_path = std::getenv("HOME");
    if (home_path) {
        return std::filesystem::path(home_path) / ".config" / config_file_name;
    }

    return std::nullopt;
}
