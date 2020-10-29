#include "websocket-session.h"

#include "utils.h"

CwWebSocketSession::CwWebSocketSession(ip::tcp::socket&& socket) :
    ws_(move(socket)) {
}


void CwWebSocketSession::accept(bx_request req) {
    // Set suggested timeout settings for the websocket
    ws_.set_option(ws::stream_base::timeout::suggested(beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    ws_.set_option(ws::stream_base::decorator([](ws::response_type& res) {
        res.set(http::field::server, kServerName + " advanced-server");
    }));

    // Accept the websocket handshake
    ws_.async_accept(req, beast::bind_front_handler(&CwWebSocketSession::onAccept, shared_from_this()));
}

void CwWebSocketSession::onAccept(beast::error_code ec) {
    if (ec)
        return fail(ec, "accept");

    // Read a message
    read();
}

void CwWebSocketSession::read() {
    // Read a message into our buffer
    ws_.async_read(buffer_, beast::bind_front_handler(&CwWebSocketSession::onRead, shared_from_this()));
}

void CwWebSocketSession::onRead(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    // This indicates that the websocket_session was closed
    if (ec == ws::error::closed)
        return;

    if (ec)
        fail(ec, "read");

    // Echo the message
    ws_.text(ws_.got_text());
    ws_.async_write(buffer_.data(), beast::bind_front_handler(&CwWebSocketSession::onWrite, shared_from_this()));
}

void CwWebSocketSession::onWrite(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec)
        return fail(ec, "write");

    // Clear the buffer
    buffer_.consume(buffer_.size());

    // Do another read
    read();
}
