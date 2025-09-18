#pragma once
#include <string>


// Login-Status
namespace login {
    extern bool loggedIn;
    extern bool loginFailed;

    bool CheckCredentials(const std::string& user, const std::string& password);
    bool RegisterDevice(const std::string& user, const std::string& password);
    bool IsDeviceRegistered();
    std::string GetRegisteredHWIDPath();
    extern std::string loginFailedMessage;
}