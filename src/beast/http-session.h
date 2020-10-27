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
        template<bool isRequest, class Body, class Fields>
        void operator()(http::message<isRequest, Body, Fields>&& msg) {
            // This holds a work item
            struct work_impl : work {
                CwHttpSession& self_;
                http::message<isRequest, Body, Fields> msg_;

                work_impl(
                    CwHttpSession& self,
                    http::message<isRequest, Body, Fields>&& msg) :
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
    };

    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    shared_ptr<const string> doc_root_;
    queue queue_;

    // The parser is stored in an optional container so we can
    // construct it from scratch it at the beginning of each new message.
    optional<http::request_parser<http::string_body>> parser_;

public:
    // Take ownership of the socket
    CwHttpSession(ip::tcp::socket&& socket, shared_ptr<const string> const& doc_root);

    // Start the session
    void run();

private:
    void do_read();
    void on_read(beast::error_code ec, size_t bytes_transferred);
    void on_write(bool close, beast::error_code ec, size_t bytes_transferred);
    void do_close();
};

#endif // HTTP_SESSION_H
