#include "sw.h"
#include <windows.h>
#include <shlobj.h>
#include <filesystem>
#include <optional>

namespace sw{

// TODO:  Windows & mac testing
std::optional<std::filesystem::path> get_config_file_path() {
    std::filesystem::path config_file_name{L"sw.ini"};

    PWSTR path {nullptr};
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, nullptr, &path);
    if (SUCCEEDED(hr)) {
        CoTaskMemFree(path);
        return std::filesystem::path(path) / L"sw" / config_file_name;
    }

    return std::nullopt;
}

} // namespace sw
