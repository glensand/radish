#include "client.h"

#include "hope-io/net/factory.h"
#include "hope-io/net/event_loop.h"
#include "hope-io/net/stream.h"
#include "hope-io/net/acceptor.h"
#include "hope-io/net/factory.h"
#include "hope-io/net/init.h"

#include "serialization.h"
#include "stream_wrapper.h"
#include "foundation.h"

#include <string>
#include <cstdint>

namespace radish {

tcp_client::tcp_client(const std::string& host, uint16_t port)
    : m_host(host), m_port(port) 
{
    radish::init();
    hope::io::init();
    m_stream = hope::io::create_stream();
}

tcp_client::~tcp_client() {
    m_stream->disconnect();
    delete m_stream;
}

} // namespace radish