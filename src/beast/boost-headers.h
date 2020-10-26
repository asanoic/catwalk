#ifndef BOOSTHEADERS_H
#define BOOSTHEADERS_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace CwAsio = boost::asio;
namespace CwBeast = boost::beast;
namespace CwHttp = CwBeast::http;
namespace CwWebSocket = CwBeast::websocket;

using CwIoContext = CwAsio::io_context;
using CwEndPoint = CwAsio::ip::tcp::endpoint;
using CwSignalSet = CwAsio::signal_set;
using CwAddress = CwAsio::ip::address;
using CwAcceptor = CwAsio::ip::tcp::acceptor;
using CwSocket = CwAsio::ip::tcp::socket;
using CwSockeBase = CwAsio::socket_base;

using CwErrorCode = CwBeast::error_code;
using CwStringBody = CwHttp::string_body;
using CwFileBody = CwHttp::file_body;
using CwEmptyBody = CwHttp::empty_body;
using CwTcpStream = CwBeast::tcp_stream;
using CwFlatBuffer = CwBeast::flat_buffer;

template<bool A, typename B, typename C>
using CwMessage = CwHttp::message<A, B, C>;
template<typename A>
using CwRequestParser = CwHttp::request_parser<A>;
template<typename A, typename B>
using CwRequest = CwHttp::request<A, B>;
template<typename A>
using CwResponse = CwHttp::response<A>;

template<typename A>
using CwWsStream = CwWebSocket::stream<A>;
using CwWsStreamBase = CwWebSocket::stream_base;
using CwWsTimeout = CwWebSocket::stream_base::timeout;


#endif // BOOSTHEADERS_H
