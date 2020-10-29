#ifndef HTTP_SESSION_H
#define HTTP_SESSION_H

#include <deque>
#include <iostream>
#include <memory>
#include <optional>

#include "boost-headers.h"

using namespace std;

// Handles an HTTP server connection
class CwHttpSession : public enable_shared_from_this<CwHttpSession> {
public:
    // Take ownership of the socket
    CwHttpSession(ip::tcp::socket&& socket, shared_ptr<const string> const& doc_root);
    // Start the session
    void run();
private:
    bx_response* handle_request(string_view doc_root, bx_request&& req) const noexcept;
    void read();
    void onRead(beast::error_code ec, size_t bytes_transferred);
    void onWrite(bool closed, beast::error_code ec, size_t bytes_transferred);
    void close();

private:
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    shared_ptr<const string> doc_root_;
    // The parser is stored in an optional container so we can
    // construct it from scratch it at the beginning of each new message.
    optional<bx_request_parser> parser_;
    unique_ptr<bx_response> responseHolder;

};

#endif // HTTP_SESSION_H
