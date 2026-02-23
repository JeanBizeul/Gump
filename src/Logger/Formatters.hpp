#include <format>
#include <filesystem>
#include <string>
#include <locale>
#include <codecvt>

template <>
struct std::formatter<std::filesystem::path> : std::formatter<std::string> {
    auto format(const std::filesystem::path &p, auto &ctx) const {
        return std::formatter<std::string>::format(p.string(), ctx);
    }
};
