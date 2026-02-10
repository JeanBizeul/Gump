#include "Utils/Utils.hpp"

#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#include <codecvt>
#include <locale>

static std::wstring utf8ToWide(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    wstr.resize(wcslen(wstr.c_str())); // trim null terminator
    return wstr;
}

static std::string wideToUtf8(const std::wstring& wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, nullptr, nullptr);
    str.resize(strlen(str.c_str())); // trim null terminator
    return str;
}


std::string Gump::Utils::openFilePickerDialog(const std::string& title, const std::string& filter)
{
    OPENFILENAMEW ofn;
    std::wstring wFileName(MAX_PATH, L'\0');

    // Convert UTF-8 to wide strings for Win32 API
    std::wstring wFilter = utf8ToWide(filter);
    std::wstring wTitle  = utf8ToWide(title);


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
        wFileName.resize(wcslen(wFileName.c_str())); // trim extra nulls
        // Convert back to UTF-8 string
        return wideToUtf8(wFileName);
    }

    return "";
}

std::string Gump::Utils::saveFilePickerDialog(const std::string& title, const std::string& filter)
{
    OPENFILENAMEW ofn;
    std::wstring wFileName(MAX_PATH, L'\0');

    std::wstring wFilter = utf8ToWide(filter);
    std::wstring wTitle  = utf8ToWide(title);

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

    // Flags for SAVE dialog
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    ofn.lpstrTitle = wTitle.c_str();

    if (GetSaveFileNameW(&ofn))
    {
        wFileName.resize(wcslen(wFileName.c_str())); 
        return wideToUtf8(wFileName);
    }

    return "";
}

#endif

#ifdef __linux__
#include <cstdio>
#include <memory>
#include <array>

std::string Gump::Utils::openFilePickerDialog(const std::string& title, const std::string& /*filter*/)
{
    std::string cmd = "zenity --file-selection --title=\"" + title + "\"";
    std::array<char, 512> buffer;
    std::string result;

    std::unique_ptr<FILE, int(*)(FILE*)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) return "";

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        result += buffer.data();

    if (!result.empty() && result.back() == '\n')
        result.pop_back();

    return result;
}


std::string Gump::Utils::saveFilePickerDialog(const std::string& title, const std::string&)
{
    std::string cmd =
        "zenity --file-selection --save --confirm-overwrite --title=\"" + title + "\"";

    std::array<char, 512> buffer;
    std::string result;

    std::unique_ptr<FILE, int(*)(FILE*)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) return "";

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        result += buffer.data();

    if (!result.empty() && result.back() == '\n')
        result.pop_back();

    return result;
}

#endif
