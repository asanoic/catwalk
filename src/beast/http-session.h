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
    // This queue is used for HTTP pipelining.
    struct queue {
        enum {
            // Maximum number of responses we will queue
            limit = 8
        };

        // The type-erased, saved work item
        struct work {
            virtual ~work() = default;
            virtual void operator()() = 0;
        };

        CwHttpSession& self_;
        deque<unique_ptr<work>> items_;

    public:
        explicit queue(CwHttpSession& self);

        // Returns `true` if we have reached the queue limit
        bool is_full() const;

        // Called when a message finishes sending
        // Returns `true` if the caller should initiate a read
        bool on_write();

        // Called by the HTTP handler to send a response.
        void operator()(bx_response&& msg);
    };

    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    shared_ptr<const string> doc_root_;
    queue queue_;

    // The parser is stored in an optional container so we can
    // construct it from scratch it at the beginning of each new message.
    optional<bx_request_parser> parser_;

public:
    // Take ownership of the socket
    CwHttpSession(ip::tcp::socket&& socket, shared_ptr<const string> const& doc_root);

    // Start the session
    void run();

private:
    void handle_request(string_view doc_root, bx_request&& req, function<void(bx_response)>&& send);
    void do_read();
    void on_read(beast::error_code ec, size_t bytes_transferred);
    void on_write(bool close, beast::error_code ec, size_t bytes_transferred);
    void do_close();
};

#endif // HTTP_SESSION_H
