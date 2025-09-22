#include <fcntl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <system_error>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#include "forwarder.h"

void ProxyClient::set_buffer(char *buffer, size_t buffer_len) {
    std::memcpy(m_buffer, buffer, buffer_len);
    m_pending_bytes = buffer_len;
    m_offset = 0;
};

void ProxyClient::advance(size_t nwrote) {
    m_pending_bytes -= nwrote;
    m_offset += nwrote;
}

ProxyHandler::ProxyHandler(ProxyClient client, ProxyClient forwarder)
    : m_client(client), m_forwarder(forwarder) {

    events.emplace_back(client.fd(), POLLIN, 0);
    events.emplace_back(forwarder.fd(), POLLIN, 0);
}

ProxyHandler::~ProxyHandler() {
    close(m_client.fd());
    close(m_forwarder.fd());
}

ProxyClient &ProxyHandler::find(int fd) noexcept {
    if (m_client.fd() == fd) {
        return m_client;
    } else {
        return m_forwarder;
    }
}

ProxyClient &ProxyHandler::get_peer(ProxyClient &client) noexcept {
    if (m_client.fd() == client.fd()) {
        return m_forwarder;
    } else {
        return m_client;
    }
}

void ProxyHandler::queue(ProxyClient &client, char *buffer, size_t buffer_len) noexcept {
    client.set_buffer(buffer, buffer_len);
    events[client.id()].events = POLLOUT;
}

void SocketForwarder::handle_client(int forwarder, int clientfd) {

    ssize_t nbytes, nsend;
    char buffer[kBufferSize];

    if (fcntl(clientfd, F_SETFL, O_NONBLOCK) < 0) {
        throw std::system_error(errno, std::generic_category(), "fcntl");
    }

    ProxyHandler handler(ProxyClient(clientfd, 0), ProxyClient(forwarder, 1));

    while (true) {
        if (poll(handler.events.data(), handler.events.size(), -1) < 0) {
            throw std::system_error(errno, std::generic_category(), "poll");
        }

        for (auto &ev : handler.events) {

            if (ev.revents & POLLOUT) {
                auto &client = handler.find(ev.fd);
                auto &peer = handler.get_peer(client);

                nsend = send(peer.fd(), client.buffer(), client.pending_bytes(), MSG_NOSIGNAL);

                if (nbytes == 0) {
                    return;
                }

                if (nsend < 0) {
                    continue;
                }

                client.advance(nsend);

                if (client.is_drained()) {
                    ev.events = POLLIN;
                }

            } else if (ev.revents & POLLIN) {
                auto &client = handler.find(ev.fd);

                nbytes = recv(client.fd(), buffer, kBufferSize, MSG_NOSIGNAL);

                if (nbytes == 0) {
                    return;
                }

                if (nbytes < 0) {
                    continue;
                }

                handler.queue(client, buffer, nbytes);

            } else if (ev.revents & (POLLRDHUP | POLLERR | POLLHUP | POLLNVAL)) {
                return;
            }
        }
    }
}

void SocketForwarder::listen(std::unique_ptr<Handler> handler) {

    int client;

    while (true) {
        if ((client = accept(handler->listener(), NULL, NULL)) < 0) {
            throw std::system_error(errno, std::generic_category(), "accept");
        }

        struct ucred cred;
        socklen_t len = sizeof(cred);

        if (getsockopt(client, SOL_SOCKET, SO_PEERCRED, &cred, &len) == -1) {
            close(client);
            throw std::system_error(errno, std::generic_category(), "getsockopt");
        }
        printf("Connected process PID: %d, UID: %d, GID: %d\n", cred.pid, cred.uid, cred.gid);

        int forwarder = handler->setup_forwarder();
        std::thread(&SocketForwarder::handle_client, this, forwarder, client).detach();
    }
}

void SocketForwarder::run() {

    std::vector<std::thread> server_threads;
    for (auto &handler : m_handlers) {
        server_threads.emplace_back(&SocketForwarder::listen, this, std::move(handler));
    }

    for (auto &thread : server_threads) {
        thread.join();
    }
}
