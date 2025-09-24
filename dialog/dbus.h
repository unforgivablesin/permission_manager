#pragma once

#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

#include <sdbus-c++/sdbus-c++.h>
#include <wx/wx.h>

enum class DbusPermission { Xorg = 0, Wayland = 1, Microphone = 2, Webcam = 3 };

struct DbusPermissionRequest {
    std::string app;
    DbusPermission permission;
    bool allow;
};

class DbusPermissionServer {
  public:
    DbusPermissionServer(wxWindow *);

    void run();
    sdbus::Struct<std::string, int, bool> handle_response(std::string, DbusPermission, bool);
    void handle_request(std::string &, DbusPermission &, bool);

  private:
    std::unique_ptr<sdbus::IConnection> m_connection;
    std::unique_ptr<sdbus::IObject> m_concatenator;
    wxWindow *m_dialog;
};

bool showPermissionPopup(wxWindow *, const std::string &, const std::string &);
