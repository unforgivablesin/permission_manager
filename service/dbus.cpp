#include <iostream>
#include <print>

#include "dbus.h"
#include "log.h"

bool dbus_ask_permission(std::string app, DbusPermission permission, bool allow) {
    sdbus::ServiceName destination{"org.permissiondialog.PermissionDialog"};
    sdbus::ObjectPath objectPath{"/org/permissiondialog/request"};
    auto concatenatorProxy = sdbus::createProxy(std::move(destination), std::move(objectPath));

    logger.log("Sending DBUS request");

    sdbus::InterfaceName interfaceName{"org.permissiondialog.PermissionDialog"};

    DbusPermissionRequest req{app, DbusPermission::Xorg, allow};
    sdbus::Struct msg = sdbus::Struct<std::string, int, bool>{req.app, static_cast<int>(req.permission), req.allow};
    sdbus::Struct response = sdbus::Struct<std::string, int, bool>{};

    concatenatorProxy->callMethod("request_permission")
        .onInterface(interfaceName)
        .withArguments(msg)
        .storeResultsTo(response);

    DbusPermissionRequest resp{response.get<0>(), static_cast<DbusPermission>(response.get<1>()), response.get<2>()};
    return resp.allow;
}
