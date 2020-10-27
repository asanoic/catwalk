#ifndef WEBSOCKET_SESSION_H
#define WEBSOCKET_SESSION_H

#include <memory>

#include "boost-headers.h"

using namespace std;

// Echoes back all received WebSocket messages
class CwWebSocketSession : public enable_shared_from_this<CwWebSocketSession> {
    ws::stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_;

public:
    // Take ownership of the socket
    explicit CwWebSocketSession(ip::tcp::socket&& socket);

    // Start the asynchronous accept operation
    template<class Body, class Allocator>
    void do_accept(http::request<Body, http::basic_fields<Allocator>> req) {
        // Set suggested timeout settings for the websocket
        ws_.set_option(ws::stream_base::timeout::suggested(beast::role_type::server));

        // Set a decorator to change the Server of the handshake
        ws_.set_option(ws::stream_base::decorator([](ws::response_type& res) {
            res.set(http::field::server, string(BOOST_BEAST_VERSION_STRING) + " advanced-server");
        }));

        // Accept the websocket handshake
        ws_.async_accept(req, beast::bind_front_handler(&CwWebSocketSession::on_accept, shared_from_this()));
    }

private:
    void on_accept(beast::error_code ec);
    void do_read();
    void on_read(beast::error_code ec, size_t bytes_transferred);
    void on_write(beast::error_code ec, size_t bytes_transferred);
};

#endif // WEBSOCKET_SESSION_H
