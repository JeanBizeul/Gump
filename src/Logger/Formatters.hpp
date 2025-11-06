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

template <>
struct std::formatter<std::wstring> : std::formatter<std::string> {
    auto format(const std::wstring &wstr, auto &ctx) const {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string str = converter.to_bytes(wstr);
        return std::formatter<std::string>::format(str, ctx);
    }
};
