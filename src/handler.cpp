#include <cstring>
#include <fcntl.h>
#include <memory>
#include <print>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <system_error>
#include <unistd.h>

#include "handler.h"

Handler::Handler(std::string_view path, std::string_view socket) : m_path(path) {
    setup_listener(socket);
    setup_forwarder();
}

Handler::~Handler() {
    if (m_listener > 0)
        close(m_listener);

    unlink(m_path.data());
}

void Handler::close_listener() noexcept {
    close(m_listener);
    m_listener = -1;
}

int Handler::setup_forwarder() {

    int forwarder;

    if ((forwarder = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        throw std::system_error(errno, std::generic_category(), "socket");
    }

    struct sockaddr_un bind_addr = {.sun_family = AF_UNIX};
    std::strncpy(bind_addr.sun_path, m_path.data(), sizeof(bind_addr.sun_path) - 1);

    if (connect(forwarder, (struct sockaddr *)&bind_addr, sizeof(struct sockaddr_un)) < 0) {
        close(forwarder);
        throw std::system_error(errno, std::generic_category(), std::format("connect {}", m_path));
    }

    if (fcntl(forwarder, F_SETFL, O_NONBLOCK) < 0) {
        close(forwarder);
        throw std::system_error(errno, std::generic_category(), "fcntl");
    }

    return forwarder;
}

void Handler::setup_listener(std::string_view socket_name) {

    if ((m_listener = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        throw std::system_error(errno, std::generic_category(), "socket");
    }

    struct sockaddr_un bind_addr = {.sun_family = AF_UNIX};
    std::snprintf(bind_addr.sun_path, sizeof(bind_addr.sun_path) - 1, "/tmp/%s",
                  socket_name.data());

    unlink(bind_addr.sun_path);

    if (bind(m_listener, (struct sockaddr *)&bind_addr, sizeof(struct sockaddr_un)) < 0) {
        close_listener();
        throw std::system_error(errno, std::generic_category(),
                                std::format("bind {}", bind_addr.sun_path));
    }

    if (listen(m_listener, 128) < 0) {
        close_listener();
        throw std::system_error(errno, std::generic_category(),
                                std::format("listen {}", bind_addr.sun_path));
    }
}

bool Handler::can_forward() noexcept { return true; }
bool PipewireHandler::can_forward() noexcept { return true; }
bool PulseAudioHandler::can_forward() noexcept { return true; }
bool DbusHandler::can_forward() noexcept { return true; }
bool WaylandHandler::can_forward() noexcept { return true; }
bool AtSpiHandler::can_forward() noexcept { return true; }
bool XorgHandler::can_forward() noexcept { return true; }
