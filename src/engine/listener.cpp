#include "listener.h"

#include "http-session.h"
#include "utils.h"

CwListener::CwListener(asio::io_context& ioc, ip::tcp::endpoint endpoint, CwFullHandler handler) :
    ioc_(ioc),
    acceptor_(asio::make_strand(ioc)),
    handler(handler) {
    beast::error_code ec;

    // Open the acceptor
    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
        fail(ec, "open");
        return;
    }

    // Allow address reuse
    acceptor_.set_option(asio::socket_base::reuse_address(true), ec);
    if (ec) {
        fail(ec, "set_option");
        return;
    }

    // Bind to the server address
    acceptor_.bind(endpoint, ec);
    if (ec) {
        fail(ec, "bind");
        return;
    }

    // Start listening for connections
    acceptor_.listen(asio::socket_base::max_listen_connections, ec);
    if (ec) {
        fail(ec, "listen");
        return;
    }
}

void CwListener::run() {
    // We need to be executing within a strand to perform async operations
    // on the I/O objects in this session. Although not strictly necessary
    // for single-threaded contexts, this example code is written to be
    // thread-safe by default.
    asio::dispatch(acceptor_.get_executor(), beast::bind_front_handler(&CwListener::accept, this));
}

void CwListener::accept() {
    // The new connection gets its own strand
    acceptor_.async_accept(asio::make_strand(ioc_), beast::bind_front_handler(&CwListener::onAccept, this));
}

void CwListener::onAccept(beast::error_code ec, ip::tcp::socket socket) {
    if (ec) {
        fail(ec, "accept");
    } else {
        // Create the http session and run it
        make_shared<CwHttpSession>(move(socket), handler)->read();
    }
    // Accept another connection
    accept();
}
