#pragma once

#include <memory>
#include <stdint.h>

class Handler {

  public:
    Handler(std::string, std::string);
    ~Handler();

    [[nodiscard]] virtual bool can_forward() noexcept;
    [[nodiscard]] int listener() noexcept { return m_listener; };
    [[nodiscard]] std::string_view path() noexcept { return m_path; };

    int setup_forwarder();

  protected:
    void handle_events(int);
    void setup_listener(std::string);

    void close_listener() noexcept;
    void close_forwarder() noexcept;

    int m_listener;
    std::string m_path;
};

template <typename T>
concept IsHandler = std::derived_from<T, Handler>;

class PulseAudioHandler : public Handler {
  public:
    PulseAudioHandler(std::string path) : Handler(path, "pulse/native") {};

    [[nodiscard]] bool can_forward() noexcept;
};

class PipewireHandler : public Handler {
  public:
    PipewireHandler(std::string path) : Handler(path, "pipewire-0") {};

    [[nodiscard]] bool can_forward() noexcept;
};

class DbusHandler : public Handler {
  public:
    DbusHandler(std::string path) : Handler(path, "bus") {};

    [[nodiscard]] bool can_forward() noexcept;
};

class WaylandHandler : public Handler {
  public:
    WaylandHandler(std::string path) : Handler(path, "wayland-0") {};

    [[nodiscard]] bool can_forward() noexcept;
};

class AtSpiHandler : public Handler {
  public:
    AtSpiHandler(std::string path) : Handler(path, "at-spi/bus") {};

    [[nodiscard]] bool can_forward() noexcept;
};

class XorgHandler : public Handler {
  public:
    XorgHandler(std::string path) : Handler(path, ".X11-unix2/X0") {};

    [[nodiscard]] bool can_forward() noexcept;
};
