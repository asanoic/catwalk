#ifndef BOOSTHEADERS_H
#define BOOSTHEADERS_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ws = beast::websocket;
namespace ip = asio::ip;

using bx_request = http::request<http::vector_body<uint8_t>>;
using bx_response = http::response<http::vector_body<uint8_t>>;

using bx_request_parser = http::request_parser<http::vector_body<uint8_t>>;
using bx_response_parser = http::response_parser<http::vector_body<uint8_t>>;

#endif // BOOSTHEADERS_H
