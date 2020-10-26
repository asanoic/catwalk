#include "http-session.h"

#include "utils.h"
#include "websocket-session.h"

CwHttpSession::CwHttpSession(CwSocket&& socket, shared_ptr<const string> const& doc_root) :
    stream_(move(socket)),
    doc_root_(doc_root),
    queue_(*this) {
}

void CwHttpSession::run() {
    // We need to be executing within a strand to perform async operations
    // on the I/O objects in this session. Although not strictly necessary
    // for single-threaded contexts, this example code is written to be
    // thread-safe by default.
    CwAsio::dispatch(stream_.get_executor(), CwBeast::bind_front_handler(&CwHttpSession::do_read, this->shared_from_this()));
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
    CwHttp::async_read(stream_, buffer_, *parser_, CwBeast::bind_front_handler(&CwHttpSession::on_read, shared_from_this()));
}

void CwHttpSession::on_read(CwErrorCode ec, size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    // This means they closed the connection
    if (ec == CwHttp::error::end_of_stream)
        return do_close();

    if (ec)
        return fail(ec, "read");

    // See if it is a WebSocket Upgrade
    if (CwWebSocket::is_upgrade(parser_->get())) {
        // Create a websocket session, transferring ownership
        // of both the socket and the HTTP request.
        std::make_shared<CwWebSocketSession>(stream_.release_socket())->do_accept(parser_->release());
        return;
    }

    // Send the response
    handle_request(*doc_root_, parser_->release(), queue_);

    // If we aren't at the queue limit, try to pipeline another request
    if (!queue_.is_full())
        do_read();
}

void CwHttpSession::on_write(bool close, CwErrorCode ec, size_t bytes_transferred) {
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
    CwErrorCode ec;
    stream_.socket().shutdown(CwSocket::shutdown_send, ec);

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
    if (!items_.empty())
        (*items_.front())();
    return was_full;
}
