#pragma once

#include <cstring>
#include <memory>
#include <string>
#include <sys/poll.h>
#include <vector>

#include "handler.h"

constexpr size_t kBufferSize = 128;

class ProxyClient {

  public:
    ProxyClient(int fd, int id) : m_fd(fd), m_id(id) {};

    [[nodiscard]] int id() noexcept { return m_id; };
    [[nodiscard]] int fd() noexcept { return m_fd; };
    [[nodiscard]] char *buffer() noexcept { return m_buffer + m_offset; };
    [[nodiscard]] size_t pending_bytes() noexcept { return m_pending_bytes; };
    [[nodiscard]] bool is_drained() { return m_pending_bytes == 0; };

    void set_buffer(char *, size_t);
    void advance(size_t nwrote);

  private:
    char m_buffer[kBufferSize];
    size_t m_pending_bytes, m_offset;
    struct pollfd *m_ev;
    int m_fd, m_id;
};

class ProxyHandler {

  public:
    ProxyHandler(ProxyClient client, ProxyClient forwarder);
    ~ProxyHandler();

    [[nodiscard]] ProxyClient &find(int) noexcept;
    [[nodiscard]] ProxyClient &get_peer(ProxyClient &) noexcept;
    void queue(ProxyClient &, char *, size_t) noexcept;

    std::vector<struct pollfd> events;

  private:
    ProxyClient m_client, m_forwarder;
};

class SocketForwarder {
  public:
    ~SocketForwarder() {};
    void run();

    template <IsHandler T> void register_path(std::string_view path) {
        auto handler = std::make_unique<T>(path);
        m_handlers.push_back(std::move(handler));
    }

  private:
    void listen(std::unique_ptr<Handler>);
    void handle_client(int, int);

    std::vector<std::unique_ptr<Handler>> m_handlers;
};
