#ifndef LISTENER_H
#define LISTENER_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;
namespace beast = boost::beast;

class listener : public std::enable_shared_from_this<listener> {
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    std::shared_ptr<std::string const> doc_root_;

public:
    listener(net::io_context& ioc, tcp::endpoint endpoint, std::shared_ptr<std::string const> const& doc_root);
    void run();

private:
    void do_accept();

    void on_accept(beast::error_code ec, tcp::socket socket);
};
#endif // LISTENER_H
