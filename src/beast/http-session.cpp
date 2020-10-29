#include "http-session.h"

#include "utils.h"
#include "websocket-session.h"

CwHttpSession::CwHttpSession(ip::tcp::socket&& socket, shared_ptr<const string> const& doc_root) :
    stream_(move(socket)),
    doc_root_(doc_root) {
}

void CwHttpSession::run() {
    // We need to be executing within a strand to perform async operations
    // on the I/O objects in this session. Although not strictly necessary
    // for single-threaded contexts, this example code is written to be
    // thread-safe by default.
    asio::dispatch(stream_.get_executor(), beast::bind_front_handler(&CwHttpSession::read, this->shared_from_this()));
}

bx_response* CwHttpSession::handle_request(string_view doc_root, bx_request&& req) const noexcept {
    auto const bad_request = [&req](string_view why) {
        bx_response* res = new bx_response(http::status::bad_request, req.version());
        res->set(http::field::server, kServerName);
        res->set(http::field::content_type, "text/html");
        res->keep_alive(req.keep_alive());
        res->body() = fromString(string(why));
        res->prepare_payload();
        return res;
    };

    // Returns a not found response
    auto const not_found = [&req](string_view target) {
        bx_response* res = new bx_response(http::status::not_found, req.version());
        res->set(http::field::server, kServerName);
        res->set(http::field::content_type, "text/html");
        res->keep_alive(req.keep_alive());
        res->body() = fromString("The resource '" + string(target) + "' was not found.");
        res->prepare_payload();
        return res;
    };

    // Returns a server error response
    auto const server_error = [&req](string_view what) {
        bx_response* res = new bx_response(http::status::internal_server_error, req.version());
        res->set(http::field::server, kServerName);
        res->set(http::field::content_type, "text/html");
        res->keep_alive(req.keep_alive());
        res->body() = fromString("An error occurred: '" + string(what) + "'");
        res->prepare_payload();
        return res;
    };

    // Make sure we can handle the method
    if (req.method() != http::verb::get && req.method() != http::verb::head)
        return bad_request("Unknown HTTP-method");

    // Request path must be absolute and not contain "..".
    if (req.target().empty() || req.target()[0] != '/' || req.target().find("..") != string_view::npos)
        return bad_request("Illegal request-target");

    // Build the path to the requested file
    string path = pathJoin(doc_root, req.target());
    if (req.target().back() == '/') path.append("index.html");

    // Attempt to open the file
    beast::error_code ec;
    beast::file_stdio fs;
    fs.open(path.c_str(), beast::file_mode::scan, ec);
    // Handle the case where the file doesn't exist
    if (ec == beast::errc::no_such_file_or_directory)
        return not_found(req.target());

    // Handle an unknown error
    if (ec) return server_error(ec.message());

    vector<uint8_t> data(fs.size(ec));
    if (ec) fail(ec, "file size");

    fs.read(data.data(), data.size(), ec);
    if (ec) fail(ec, "file read");

    // Respond to HEAD request
    if (req.method() == http::verb::head) {
        bx_response* res = new bx_response(http::status::ok, req.version());
        res->set(http::field::server, kServerName);
        res->set(http::field::content_type, mimeType(path));
        res->content_length(data.size());
        res->keep_alive(req.keep_alive());
        return res;
    }

    // Respond to GET request
    bx_response* res = new bx_response(http::status::ok, req.version());
    res->set(http::field::server, kServerName);
    res->set(http::field::content_type, mimeType(path));
    res->content_length(data.size());
    res->body() = move(data);
    res->keep_alive(req.keep_alive());
    return res;
}

void CwHttpSession::read() {
    // Construct a new parser for each message
    parser_.emplace();

    // Apply a reasonable limit to the allowed size
    // of the body in bytes to prevent abuse.
    parser_->body_limit(10000);

    // Set the timeout.
    stream_.expires_after(chrono::seconds(30));

    // Read a request using the parser-oriented interface
    http::async_read(stream_, buffer_, *parser_, beast::bind_front_handler(&CwHttpSession::onRead, shared_from_this()));
}

void CwHttpSession::onRead(beast::error_code ec, size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    // This means they closed the connection
    if (ec == http::error::end_of_stream)
        return close();

    if (ec)
        return fail(ec, "read");

    // See if it is a WebSocket Upgrade
    if (ws::is_upgrade(parser_->get())) {
        // Create a websocket session, transferring ownership
        // of both the socket and the HTTP request.
        std::make_shared<CwWebSocketSession>(stream_.release_socket())->accept(parser_->release());
        return;
    }

    responseHolder.reset(handle_request(*doc_root_, parser_->release()));
    http::async_write(stream_, *responseHolder, beast::bind_front_handler(&CwHttpSession::onWrite, shared_from_this(), responseHolder->need_eof()));
}

void CwHttpSession::onWrite(bool closed, beast::error_code ec, size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec)
        return fail(ec, "write");

    if (closed) {
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        close();
    } else {
        // Inform the queue that a write completed
        // Read another request
        read();
    }
}

void CwHttpSession::close() {
    // Send a TCP shutdown
    beast::error_code ec;
    stream_.socket().shutdown(ip::tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
}
