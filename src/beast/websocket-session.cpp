#include "websocket-session.h"

#include "utils.h"

CwWebSocketSession::CwWebSocketSession(CwSocket&& socket) :
    ws_(move(socket)) {
}

void CwWebSocketSession::on_accept(CwErrorCode ec) {
    if (ec)
        return fail(ec, "accept");

    // Read a message
    do_read();
}

void CwWebSocketSession::do_read() {
    // Read a message into our buffer
    ws_.async_read(buffer_, CwBeast::bind_front_handler(&CwWebSocketSession::on_read, shared_from_this()));
}

void CwWebSocketSession::on_read(CwErrorCode ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    // This indicates that the websocket_session was closed
    if (ec == CwWebSocket::error::closed)
        return;

    if (ec)
        fail(ec, "read");

    // Echo the message
    ws_.text(ws_.got_text());
    ws_.async_write(buffer_.data(), CwBeast::bind_front_handler(&CwWebSocketSession::on_write, shared_from_this()));
}

void CwWebSocketSession::on_write(CwErrorCode ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec)
        return fail(ec, "write");

    // Clear the buffer
    buffer_.consume(buffer_.size());

    // Do another read
    do_read();
}
