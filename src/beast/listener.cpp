#include "listener.h"

#include "http-session.h"
#include "utils.h"

CwListener::CwListener(CwIoContext& ioc, CwEndPoint endpoint, shared_ptr<const string> const& doc_root) :
    ioc_(ioc),
    acceptor_(CwAsio::make_strand(ioc)),
    doc_root_(doc_root) {
    CwErrorCode ec;

    // Open the acceptor
    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
        fail(ec, "open");
        return;
    }

    // Allow address reuse
    acceptor_.set_option(CwSockeBase::reuse_address(true), ec);
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
    acceptor_.listen(CwSockeBase::max_listen_connections, ec);
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
    CwAsio::dispatch(acceptor_.get_executor(), CwBeast::bind_front_handler(&CwListener::do_accept, this->shared_from_this()));
}

void CwListener::do_accept() {
    // The new connection gets its own strand
    acceptor_.async_accept(CwAsio::make_strand(ioc_), CwBeast::bind_front_handler(&CwListener::on_accept, shared_from_this()));
}

void CwListener::on_accept(CwErrorCode ec, CwSocket socket) {
    if (ec) {
        fail(ec, "accept");
    } else {
        // Create the http session and run it
        std::make_shared<CwHttpSession>(move(socket), doc_root_)->run();
    }

    // Accept another connection
    do_accept();
}
