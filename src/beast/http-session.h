#ifndef HTTP_SESSION_H
#define HTTP_SESSION_H

#include <deque>
#include <iostream>
#include <memory>
#include <optional>
using namespace std;

#include "boost-headers.h"
#include "CwRouter.h"

// Handles an HTTP server connection
class CwHttpSession : public enable_shared_from_this<CwHttpSession> {
public:
    // Take ownership of the socket
    CwHttpSession(ip::tcp::socket&& socket, CwHandler handler);
    void read();

private:
    unique_ptr<bx_response> handle_request(string_view doc_root, bx_request&& req) const noexcept;
    void onRead(beast::error_code ec, size_t bytes_transferred);
    void onWrite(shared_ptr<bx_response> res, beast::error_code ec, size_t bytes_transferred);
    void close();

private:
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    CwHandler handler;
    // The parser is stored in an optional container so we can
    // construct it from scratch it at the beginning of each new message.
    optional<bx_request_parser> parser_;
};

#endif // HTTP_SESSION_H
