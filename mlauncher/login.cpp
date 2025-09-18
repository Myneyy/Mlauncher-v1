#include "login.h"
#include <windows.h>
#include <iphlpapi.h>
#include <shlobj.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <wincrypt.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "crypt32.lib")

namespace login {

    bool loggedIn = false;
    bool loginFailed = false;
    std::string loginFailedMessage = "";

    // ---- Helfer: SHA256 mit CryptoAPI ----
    static std::string Sha256Hex(const std::string& data) {
        HCRYPTPROV hProv = 0;
        HCRYPTHASH hHash = 0;
        std::string result;

        if (!CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
            return {};
        }
        if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
            CryptReleaseContext(hProv, 0);
            return {};
        }
        if (!CryptHashData(hHash, reinterpret_cast<const BYTE*>(data.data()), (DWORD)data.size(), 0)) {
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            return {};
        }

        BYTE hash[32];
        DWORD hashLen = sizeof(hash);
        if (CryptGetHashParam(hHash, HP_HASHVAL, hash, &hashLen, 0)) {
            std::ostringstream oss;
            for (DWORD i = 0; i < hashLen; ++i) {
                oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
            }
            result = oss.str();
        }

        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return result;
    }

    // ---- HWID Komponenten ----
    static std::string GetVolumeSerial() {
        DWORD serial = 0;
        if (GetVolumeInformationA("C:\\", NULL, 0, &serial, NULL, NULL, NULL, 0)) {
            std::ostringstream oss;
            oss << std::hex << std::setw(8) << std::setfill('0') << serial;
            return oss.str();
        }
        return "unknownvol";
    }

    static std::string GetFirstMAC() {
        IP_ADAPTER_INFO adapterInfo[16];
        DWORD buflen = sizeof(adapterInfo);
        if (GetAdaptersInfo(adapterInfo, &buflen) == NO_ERROR) {
            PIP_ADAPTER_INFO pAdapter = adapterInfo;
            // Suche Adapter mit MAC
            while (pAdapter) {
                if (pAdapter->AddressLength == 6) {
                    std::ostringstream oss;
                    for (UINT i = 0; i < pAdapter->AddressLength; ++i) {
                        if (i) oss << ":";
                        oss << std::hex << std::setw(2) << std::setfill('0') << (int)pAdapter->Address[i];
                    }
                    return oss.str();
                }
                pAdapter = pAdapter->Next;
            }
        }
        return "unknownmac";
    }

    // ---- HWID zusammensetzen und hashen ----
    static std::string GetRawHWID() {
        std::string vol = GetVolumeSerial();
        std::string mac = GetFirstMAC();
        std::string raw = "VOL:" + vol + "|MAC:" + mac;
        return raw;
    }

    std::string GetCurrentHWIDHash() {
        std::string raw = GetRawHWID();
        return Sha256Hex(raw);
    }

    // ---- Dateipfade ----
    std::string GetRegisteredHWIDPath() {
        char path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
            std::string p = std::string(path) + "\\MyApp";
            CreateDirectoryA(p.c_str(), NULL); // falls nicht existiert
            return p + "\\hwid.dat";
        }
        // Fallback: aktuelles Verzeichnis
        return "hwid.dat";
    }

    // ---- Registrierung speichern/prüfen ----
    bool SaveHWIDToFile(const std::string& hwidHash) {
        std::string fpath = GetRegisteredHWIDPath();
        std::ofstream ofs(fpath, std::ios::binary | std::ios::out);
        if (!ofs) return false;
        ofs << hwidHash;
        ofs.close();
        return true;
    }

    bool ReadHWIDFromFile(std::string& outHash) {
        std::string fpath = GetRegisteredHWIDPath();
        std::ifstream ifs(fpath, std::ios::binary);
        if (!ifs) return false;
        std::ostringstream ss;
        ss << ifs.rdbuf();
        outHash = ss.str();
        return true;
    }

    bool IsDeviceRegistered() {
        std::string stored;
        if (!ReadHWIDFromFile(stored)) return false;
        std::string current = GetCurrentHWIDHash();
        return !stored.empty() && stored == current;
    }

    // ---- Einfache Auth (nur Demo). Du kannst das ersetzen mit DB/Server. ----
    static bool CheckUserPassSimple(const std::string& user, const std::string& password) {
        // Beispiel: lokal hart kodiert (nur Demo). Ersetze mit echter Prüflogik.
        return (user == "1" && password == "1");
    }

    bool RegisterDevice(const std::string& user, const std::string& password) {
        // 1. Prüfen, ob Gerät schon registriert
        std::ifstream ifs(GetRegisteredHWIDPath(), std::ios::binary);
        if (ifs.good()) { // Datei existiert schon
            loggedIn = false;
            loginFailed = true;
            loginFailedMessage = "Device already registered!";
            return false;
        }

        // 2. Prüfen, ob User/Pass leer sind
        if (user.empty() || password.empty()) {
            loggedIn = false;
            loginFailed = true;
            loginFailedMessage = "Please enter username and password!";
            return false;
        }

        // 3. HWID holen
        std::string hwid = GetCurrentHWIDHash();
        if (hwid.empty()) {
            loggedIn = false;
            loginFailed = true;
            loginFailedMessage = "Error getting HWID!";
            return false;
        }

        // 4. Alles in Datei speichern: user:pass:hwid
        std::ofstream ofs(GetRegisteredHWIDPath(), std::ios::binary | std::ios::out);
        if (!ofs) {
            loggedIn = false;
            loginFailed = true;
            loginFailedMessage = "Error writing file!";
            return false;
        }

        ofs << user << ":" << password << ":" << hwid;
        ofs.close();

        loggedIn = true;
        loginFailed = false;
        return true;
    }

    bool CheckCredentials(const std::string& user, const std::string& password) {
        std::ifstream ifs(GetRegisteredHWIDPath(), std::ios::binary);
        if (!ifs) { // Datei existiert nicht -> noch nicht registriert
            loggedIn = false;
            loginFailed = true;
            return false;
        }

        std::string line;
        std::getline(ifs, line);
        ifs.close();

        std::string storedUser, storedPass, storedHWID;
        std::istringstream iss(line);
        std::getline(iss, storedUser, ':');
        std::getline(iss, storedPass, ':');
        std::getline(iss, storedHWID, ':');

        std::string currentHWID = GetCurrentHWIDHash();

        if (user == storedUser && password == storedPass && currentHWID == storedHWID) {
            loggedIn = true;
            loginFailed = false;
            return true;
        }
        else {
            loggedIn = false;
            loginFailed = true;
            return false;
        }
    }
}
