#include <iostream>

#include "forwarder.h"

int main() {
    SocketForwarder forwarder;

    forwarder.register_path<XorgHandler>("/tmp/.X11-unix/X0");
    // forwarder.register_path<PipewireHandler>("/run/user/1000/pipewire-0");
    // forwarder.register_path<DbusHandler>("/run/user/1000/bus");
    // forwarder.register_path<WaylandHandler>("/run/user/1000/wayland-0");

    forwarder.run();

    return 0;
}
