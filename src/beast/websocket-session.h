#ifndef WEBSOCKET_SESSION_H
#define WEBSOCKET_SESSION_H

#include <memory>

#include "boost-headers.h"

using namespace std;

// Echoes back all received WebSocket messages
class CwWebSocketSession : public enable_shared_from_this<CwWebSocketSession> {
    CwWsStream<CwTcpStream> ws_;
    CwFlatBuffer buffer_;

public:
    // Take ownership of the socket
    explicit CwWebSocketSession(CwSocket&& socket);

    // Start the asynchronous accept operation
    template<class Body, class Allocator>
    void do_accept(CwRequest<Body, CwHttp::basic_fields<Allocator>> req) {
        // Set suggested timeout settings for the websocket
        ws_.set_option(CwWsTimeout::suggested(CwBeast::role_type::server));

        // Set a decorator to change the Server of the handshake
        ws_.set_option(CwWsStreamBase::decorator([](CwWebSocket::response_type& res) {
            res.set(CwHttp::field::server, string(BOOST_BEAST_VERSION_STRING) + " advanced-server");
        }));

        // Accept the websocket handshake
        ws_.async_accept(req, CwBeast::bind_front_handler(&CwWebSocketSession::on_accept, shared_from_this()));
    }

private:
    void on_accept(CwErrorCode ec);
    void do_read();
    void on_read(CwErrorCode ec, size_t bytes_transferred);
    void on_write(CwErrorCode ec, size_t bytes_transferred);
};

#endif // WEBSOCKET_SESSION_H
