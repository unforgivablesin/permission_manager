#pragma once

#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

#include <sdbus-c++/sdbus-c++.h>

enum class DbusPermission { Xorg = 0, Wayland = 1, Microphone = 2, Webcam = 3 };

struct DbusPermissionRequest {
    std::string app;
    DbusPermission permission;
    bool allow;
};

bool dbus_ask_permission(std::string, DbusPermission, bool);
