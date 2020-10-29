#ifndef WEBSOCKET_SESSION_H
#define WEBSOCKET_SESSION_H

#include <memory>

#include "boost-headers.h"
#include "utils.h"

using namespace std;

// Echoes back all received WebSocket messages
class CwWebSocketSession : public enable_shared_from_this<CwWebSocketSession> {
public:
    // Take ownership of the socket
    explicit CwWebSocketSession(ip::tcp::socket&& socket);
    // Start the asynchronous accept operation
    void accept(bx_request req);

private:
    void onAccept(beast::error_code ec);
    void read();
    void onRead(beast::error_code ec, size_t bytes_transferred);
    void onWrite(beast::error_code ec, size_t bytes_transferred);

private:
    ws::stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_;
};

#endif // WEBSOCKET_SESSION_H
