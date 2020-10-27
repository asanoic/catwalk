#include "http-session.h"

#include "utils.h"
#include "websocket-session.h"

CwHttpSession::CwHttpSession(ip::tcp::socket&& socket, shared_ptr<const string> const& doc_root) :
    stream_(move(socket)),
    doc_root_(doc_root),
    queue_(*this) {
}

void CwHttpSession::run() {
    // We need to be executing within a strand to perform async operations
    // on the I/O objects in this session. Although not strictly necessary
    // for single-threaded contexts, this example code is written to be
    // thread-safe by default.
    asio::dispatch(stream_.get_executor(), beast::bind_front_handler(&CwHttpSession::do_read, this->shared_from_this()));
}

void CwHttpSession::do_read() {
    // Construct a new parser for each message
    parser_.emplace();

    // Apply a reasonable limit to the allowed size
    // of the body in bytes to prevent abuse.
    parser_->body_limit(10000);

    // Set the timeout.
    stream_.expires_after(chrono::seconds(30));

    // Read a request using the parser-oriented interface
    http::async_read(stream_, buffer_, *parser_, beast::bind_front_handler(&CwHttpSession::on_read, shared_from_this()));
}

void CwHttpSession::on_read(beast::error_code ec, size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    // This means they closed the connection
    if (ec == http::error::end_of_stream)
        return do_close();

    if (ec)
        return fail(ec, "read");

    // See if it is a WebSocket Upgrade
    if (ws::is_upgrade(parser_->get())) {
        // Create a websocket session, transferring ownership
        // of both the socket and the HTTP request.
        std::make_shared<CwWebSocketSession>(stream_.release_socket())->do_accept(parser_->release());
        return;
    }

    //    auto sender = [this](auto&& msg) {
    //        http::async_write(this->stream_, msg, beast::bind_front_handler(&CwHttpSession::on_write, this->shared_from_this(), msg.need_eof()));
    //    };

    // Send the response
    handle_request(*doc_root_, parser_->release(), queue_);

    if (!queue_.is_full()) do_read();
}

void CwHttpSession::on_write(bool close, beast::error_code ec, size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec)
        return fail(ec, "write");

    if (close) {
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        return do_close();
    }

    // Inform the queue that a write completed
    if (queue_.on_write()) {
        // Read another request
        do_read();
    }
}

void CwHttpSession::do_close() {
    // Send a TCP shutdown
    beast::error_code ec;
    stream_.socket().shutdown(ip::tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
}

CwHttpSession::queue::queue(CwHttpSession& self) :
    self_(self) {
    static_assert(limit > 0, "queue limit must be positive");
}

bool CwHttpSession::queue::is_full() const {
    return items_.size() >= limit;
}

bool CwHttpSession::queue::on_write() {
    BOOST_ASSERT(!items_.empty());
    auto const was_full = is_full();
    items_.pop_front();
    if (!items_.empty()) {
        (*items_.front())();
    }
    return was_full;
}
