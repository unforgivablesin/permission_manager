#pragma once

#include <cstring>
#include <memory>
#include <string>
#include <sys/poll.h>
#include <vector>

#include "client.h"
#include "handler.h"

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

    template <IsHandler T> void register_path(std::string path) {
        auto handler = std::make_unique<T>(path);
        m_handlers.push_back(std::move(handler));
    }

  private:
    void listen(std::unique_ptr<Handler>);
    void handle_client(int, int);

    std::vector<std::unique_ptr<Handler>> m_handlers;
};
