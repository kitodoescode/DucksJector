#include <windows.h>
#include <wincrypt.h>
#include <shlobj.h>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>

class SecureStorage {
private:
    static constexpr const wchar_t* folderName = L"Syrix";
    static constexpr const wchar_t* fileName = L".sys_config.dat";

    static std::wstring GetFolderPath() {
        wchar_t* localAppData;
        SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &localAppData);
        std::wstring path(localAppData);
        CoTaskMemFree(localAppData);
        path += L"\\" + std::wstring(folderName);
        return path;
    }

    static std::wstring GetFilePath() {
        return GetFolderPath() + L"\\" + std::wstring(fileName);
    }

    static void EnsureFolderExists() {
        std::wstring folder = GetFolderPath();
        CreateDirectoryW(folder.c_str(), NULL);
        SetFileAttributesW(folder.c_str(), FILE_ATTRIBUTE_HIDDEN);
    }

public:
    static void StoreSecureData(const std::string& data) {
        try {
            EnsureFolderExists();

            DATA_BLOB inBlob;
            inBlob.pbData = (BYTE*)data.data();
            inBlob.cbData = (DWORD)data.size();

            DATA_BLOB outBlob;
            if (CryptProtectData(&inBlob, L"Syrix", nullptr, nullptr, nullptr, 0, &outBlob)) {
                std::wstring path = GetFilePath();
                std::ofstream file(path, std::ios::binary);
                file.write((char*)outBlob.pbData, outBlob.cbData);
                file.close();

                SetFileAttributesW(path.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
                LocalFree(outBlob.pbData);
            }
        }
        catch (...) {}
    }

    static std::string RetrieveSecureData() {
        try {
            std::wstring path = GetFilePath();
            std::ifstream file(path, std::ios::binary);
            if (!file.good()) return "";

            std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();

            DATA_BLOB inBlob;
            inBlob.pbData = (BYTE*)buffer.data();
            inBlob.cbData = (DWORD)buffer.size();

            DATA_BLOB outBlob;
            if (CryptUnprotectData(&inBlob, nullptr, nullptr, nullptr, nullptr, 0, &outBlob)) {
                std::string result((char*)outBlob.pbData, outBlob.cbData);
                LocalFree(outBlob.pbData);
                return result;
            }
        }
        catch (...) {}
        return "";
    }

    static void DeleteSecureData() {
        try {
            std::wstring path = GetFilePath();
            DeleteFileW(path.c_str());
        }
        catch (...) {}
    }
};
