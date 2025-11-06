#include "UI/UI.hpp"

#include <string>

#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#include <codecvt>
#include <locale>
#endif

#ifdef __linux__
#include <cstdio>
#include <memory>
#include <array>
#endif

namespace Gump::UI
{

#ifdef _WIN32
std::wstring openFilePickerDialog(const std::string& title, const std::string& filter)
{
    OPENFILENAMEW ofn;
    std::wstring wFileName(MAX_PATH, L'\0');
    std::wstring wFilter;
    std::wstring wTitle;

    // Convert filter and title to wide strings
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    wFilter = conv.from_bytes(filter);
    wTitle = conv.from_bytes(title);

    // Replace '|' with '\0' for Win32 filter
    for (auto& ch : wFilter)
        if (ch == L'|') ch = L'\0';

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = &wFileName[0];
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = wFilter.c_str();
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.lpstrTitle = wTitle.c_str();

    if (GetOpenFileNameW(&ofn))
    {
        wFileName.resize(wcslen(wFileName.c_str())); // remove extra nulls
        return wFileName;
    }

    return L"";
}
#endif

#ifdef __linux__
std::wstring openFilePickerDialog(const std::string& title, const std::string& /*filter*/)
{
    std::string cmd = "zenity --file-selection --title=\"" + title + "\"";
    std::array<char, 512> buffer;
    std::string result;

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) return "";

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        result += buffer.data();

    if (!result.empty() && result.back() == '\n')
        result.pop_back();

    return result;
}
#endif

} // namespace Gump::UI
