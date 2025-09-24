#include <iostream>
#include <print>

#include "dbus.h"

void on_request_permission(const std::string &concatenatedString) {
    std::cout << "Received signal with concatenated string " << concatenatedString << std::endl;
}

DbusPermissionServer::DbusPermissionServer(wxWindow *dialog) : m_dialog(dialog) {
    sdbus::ServiceName serviceName{"org.permissiondialog.PermissionDialog"};
    m_connection = sdbus::createBusConnection(serviceName);

    sdbus::ObjectPath objectPath{"/org/permissiondialog/request"};
    m_concatenator = sdbus::createObject(*m_connection, std::move(objectPath));

    m_concatenator
        ->addVTable(sdbus::registerMethod("request_permission")
                        .implementedAs([this](sdbus::Struct<std::string, int, bool> req) {
                            return handle_response(req.get<0>(), static_cast<DbusPermission>(req.get<1>()),
                                                   req.get<2>());
                        }),
                    sdbus::registerSignal("request_permission").withParameters<std::string>())
        .forInterface("org.permissiondialog.PermissionDialog");
}

sdbus::Struct<std::string, int, bool> DbusPermissionServer::handle_response(std::string app, DbusPermission permission,
                                                                            bool allow) {

    bool result = showPermissionPopup(m_dialog, app, "access the Xorg");

    sdbus::Struct response = sdbus::Struct<std::string, int, bool>{app, static_cast<int>(permission), result};
    return response;
}

void DbusPermissionServer::run() { m_connection->enterEventLoop(); }
