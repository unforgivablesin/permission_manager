#include <cstring>

#include "client.h"

void ProxyClient::set_buffer(char *buffer, size_t buffer_len) {
    std::memcpy(m_buffer, buffer, buffer_len);
    m_pending_bytes = buffer_len;
    m_offset = 0;
};

void ProxyClient::advance(size_t nwrote) {
    m_pending_bytes -= nwrote;
    m_offset += nwrote;
}
