#ifndef LISTENER_H
#define LISTENER_H

#include <memory>
#include <string>

#include "boost-headers.h"

using namespace std;

class CwListener {
public:
    CwListener(asio::io_context& ioc, ip::tcp::endpoint endpoint,shared_ptr<const string> const& doc_root);
    void run();

private:
    void accept();
    void onAccept(beast::error_code ec, ip::tcp::socket socket);

private:
    asio::io_context& ioc_;
    ip::tcp::acceptor acceptor_;
    shared_ptr<const string> doc_root_;
};
#endif // LISTENER_H
