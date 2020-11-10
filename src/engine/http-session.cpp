#include "http-session.h"

#include "CwRequestData.h"
#include "CwResponseData.h"
#include "utils.h"
#include "websocket-session.h"

CwHttpSession::CwHttpSession(ip::tcp::socket&& socket, CwHandler handler) :
    stream_(move(socket)),
    handler(handler) {
}

void CwHttpSession::read() {
    auto request = make_unique<CwRequest>();
    CW_GET_DATAEX(d, CwRequest, request);
    d->beastRequestParser.body_limit(10000);

    // Set the timeout.
    stream_.expires_after(chrono::seconds(30));

    // Read a request using the parser-oriented interface
    http::async_read(stream_, buffer_, d->beastRequestParser, beast::bind_front_handler(&CwHttpSession::onRead, shared_from_this(), move(request)));
}

void CwHttpSession::onRead(unique_ptr<CwRequest> request, beast::error_code ec, size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    // This means they closed the connection
    if (ec == http::error::end_of_stream) return close();
    if (ec) return fail(ec, "read");

    // See if it is a WebSocket Upgrade
    //    if (ws::is_upgrade(d->beastRequestParser)) {
    //        // Create a websocket session, transferring ownership
    //        // of both the socket and the HTTP request.
    //        std::make_shared<CwWebSocketSession>(stream_.release_socket())->accept(parser_->release());

    //    } else {
    auto response = make_unique<CwResponse>();
    CW_GET_DATAEX(dq, CwRequest, request);
    CW_GET_DATAEX(dp, CwResponse, response);
    handler(request.get(), response.get());
    dp->beastResponse.prepare_payload();
    dp->beastResponse.keep_alive(dq->beastRequestParser.keep_alive());
    http::async_write(stream_, dp->beastResponse, beast::bind_front_handler(&CwHttpSession::onWrite, shared_from_this(), move(response)));
    //    }
}

void CwHttpSession::onWrite(unique_ptr<CwResponse> response, beast::error_code ec, size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    CW_GET_DATAEX(d, CwResponse, response);
    if (ec) fail(ec, "write");
    if (d->beastResponse.need_eof()) return close();
    read();
}

void CwHttpSession::close() {
    // Send a TCP shutdown
    beast::error_code ec;
    stream_.socket().shutdown(ip::tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
}
