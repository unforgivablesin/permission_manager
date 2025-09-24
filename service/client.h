#pragma once

#include <unistd.h>

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
