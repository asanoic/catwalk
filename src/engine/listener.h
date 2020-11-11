#ifndef LISTENER_H
#define LISTENER_H

#include <memory>
#include <string>
using namespace std;

#include "boost-headers.h"
#include "CwRouterData.h"

class CwListener {
public:
    CwListener(asio::io_context& ioc, ip::tcp::endpoint endpoint, CwFullHandler handler);
    void run();

private:
    void accept();
    void onAccept(beast::error_code ec, ip::tcp::socket socket);

private:
    asio::io_context& ioc_;
    ip::tcp::acceptor acceptor_;
    CwFullHandler handler;
};
#endif // LISTENER_H
