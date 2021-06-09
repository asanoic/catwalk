#include "http-session.h"

#include "CwRequestData.h"
#include "CwResponseData.h"
#include "utils.h"
#include "websocket-session.h"

CwHttpSession::CwHttpSession(ip::tcp::socket&& socket, CwFullHandler handler) :
    stream_(move(socket)),
    handler(handler) {
}

void CwHttpSession::read() {
    auto request = make_unique<CwRequest>();
    CW_GET_DATAEX(d, CwRequest, request.get());
    d->beastRequestParser.emplace();
    d->beastRequestParser->body_limit(10000);
    // Set the timeout.
    stream_.expires_after(chrono::seconds(10));

    // Read a request using the parser-oriented interface
    http::async_read(stream_, buffer_, *d->beastRequestParser, beast::bind_front_handler(&CwHttpSession::onRead, shared_from_this(), move(request)));
}

void CwHttpSession::onRead(unique_ptr<CwRequest> request, beast::error_code ec, size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    CW_GET_DATAEX(dq, CwRequest, request.get());

    // This means they closed the connection
    if (ec == http::error::end_of_stream) return close();
    if (ec == beast::error::timeout && dq->beastRequestParser->get().target().empty()) return;
    if (ec) return fail(ec, "read", (char*)__FILE__, __LINE__);

    // See if it is a WebSocket Upgrade
    if (ws::is_upgrade(dq->beastRequestParser->get())) {
        // Create a websocket session, transferring ownership
        // of both the socket and the HTTP request.
        std::make_shared<CwWebSocketSession>(stream_.release_socket())->accept(dq->beastRequestParser->release());

    } else {
        dq->prepareData();
        auto response = make_unique<CwResponse>();
        CW_GET_DATAEX(dp, CwResponse, response.get());
        handler(request.get(), response.get(), CwNextFunc());
        if (!dp->sent) {
            cout << "missing call CwResponse::send()?" << endl;
            response->setStatus((int)http::status::not_found)
                ->setHeader("Content-Type", "text/html")
                ->setBody(fromString("404"))
                ->send();
        }
        dp->beastResponse.prepare_payload();
        dp->beastResponse.keep_alive(dq->beastRequestParser->keep_alive());
        http::async_write(stream_, dp->beastResponse, beast::bind_front_handler(&CwHttpSession::onWrite, shared_from_this(), move(response)));
    }
}

void CwHttpSession::onWrite(unique_ptr<CwResponse> response, beast::error_code ec, size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    CW_GET_DATAEX(d, CwResponse, response.get());
    if (ec) fail(ec, "write", (char*)__FILE__, __LINE__);
    if (d->beastResponse.need_eof()) return close();
    read();
}

void CwHttpSession::close() {
    // Send a TCP shutdown
    beast::error_code ec;
    stream_.socket().shutdown(ip::tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
}
