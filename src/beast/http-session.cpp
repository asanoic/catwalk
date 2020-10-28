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

void CwHttpSession::handle_request(string_view doc_root, bx_request&& req, function<void(bx_response)>&& send) {
    auto const bad_request = [&req](string_view why) {
        bx_response res{http::status::bad_request, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = fromString(string(why));
        res.prepare_payload();
        return res;
    };

    // Returns a not found response
    auto const not_found = [&req](string_view target) {
        bx_response res{http::status::not_found, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = fromString("The resource '" + string(target) + "' was not found.");
        res.prepare_payload();
        return res;
    };

    // Returns a server error response
    auto const server_error = [&req](string_view what) {
        bx_response res{http::status::internal_server_error, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = fromString("An error occurred: '" + string(what) + "'");
        res.prepare_payload();
        return res;
    };

    // Make sure we can handle the method
    if (req.method() != http::verb::get && req.method() != http::verb::head)
        return send(bad_request("Unknown HTTP-method"));

    // Request path must be absolute and not contain "..".
    if (req.target().empty() || req.target()[0] != '/' || req.target().find("..") != string_view::npos)
        return send(bad_request("Illegal request-target"));

    // Build the path to the requested file
    string path = path_cat(doc_root, req.target());
    if (req.target().back() == '/') path.append("index.html");

    // Attempt to open the file
    beast::error_code ec;
    beast::file_stdio fs;
    fs.open(path.c_str(), beast::file_mode::scan, ec);
    // Handle the case where the file doesn't exist
    if (ec == beast::errc::no_such_file_or_directory)
        return send(not_found(req.target()));

    // Handle an unknown error
    if (ec)
        return send(server_error(ec.message()));

    vector<uint8_t> data(fs.size(ec));
    fs.read(data.data(), data.size(), ec);

    // Respond to HEAD request
    if (req.method() == http::verb::head) {
        bx_response res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(path));
        res.content_length(data.size());
        res.keep_alive(req.keep_alive());
        return send(move(res));
    }

    // Respond to GET request
    bx_response res{http::status::ok, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(path));
    res.content_length(data.size());
    res.body() = move(data);
    res.keep_alive(req.keep_alive());
    return send(move(res));
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
    // ::handle_request(*doc_root_, parser_->release(), queue_);
    handle_request(*doc_root_, parser_->release(), [this](bx_response&& res) {
        this->queue_(move(res));
    });

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

void CwHttpSession::queue::operator()(bx_response&& msg) {
    // This holds a work item
    struct work_impl : work {
        CwHttpSession& self_;
        bx_response msg_;

        work_impl(
            CwHttpSession& self,
            bx_response&& msg) :
            self_(self),
            msg_(move(msg)) {
        }

        void operator()() {
            http::async_write(self_.stream_, msg_, beast::bind_front_handler(&CwHttpSession::on_write, self_.shared_from_this(), msg_.need_eof()));
        }
    };

    // Allocate and store the work
    items_.push_back(boost::make_unique<work_impl>(self_, std::move(msg)));

    // If there was no previous work, start this one
    if (items_.size() == 1) (*items_.front())();
}
