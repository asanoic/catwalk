#include "CwApplication.h"
#include "CwApplicationData.h"

#include <iostream>
#include <memory>
#include <thread>
using namespace std;

#include "engine/listener.h"
#include "engine/utils.h"

void __attribute__((constructor)) initForOpenFileLimited() {
#ifdef __MINGW64__
    _setmaxstdio(2048);
#endif
}

CW_OBJECT_CONSTRUCTOR(CwApplication, CwRouter) {
    CW_GET_DATA(CwApplication);
    d->threads = max<int>(1, thread::hardware_concurrency());
}

int CwApplication::start(uint16_t port) noexcept {
    CW_GET_DATA(CwApplication);

    asio::io_context ioc(d->threads);

    auto listener = make_unique<CwListener>(
        ioc,
        ip::tcp::endpoint(ip::address(), port),
        bind(&CwApplicationData::handler, d, placeholders::_1, placeholders::_2, CwNextFunc()));

    listener->run();

    asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(bind(&asio::io_context::stop, &ioc));

    vector<thread> v;
    v.reserve(d->threads - 1);
    for (auto i = d->threads - 1; i > 0; --i)
        v.emplace_back(&asio::io_context::run, &ioc);
    ioc.run();

    // If we get here, it means we got a SIGINT or SIGTERM
    for (auto& t : v) t.join();

    return EXIT_SUCCESS;
}

int CwApplication::threads() noexcept {
    CW_GET_DATA(CwApplication);
    return d->threads;
}

void CwApplication::demo(CwRequest* req, CwResponse* res, CwNextFunc next) {
    cout << req->path() << endl;
    if (req->method() != CwHttpVerb::get && req->method() != CwHttpVerb::get) {
        res->setStatus((int)http::status::bad_request)
            ->setHeader("Content-Type", "text/html")
            ->setBody(fromString("Bad Request"))
            ->send();
        return;
    }

    // Request path must be absolute and not contain "..".
    if (req->path().empty() || req->path()[0] != '/' || req->path().find("..") != string_view::npos) {
        res->setStatus((int)http::status::bad_request)
            ->setHeader("Content-Type", "text/html")
            ->setBody(fromString("Illegal request-target"))
            ->send();
        return;
    }

    string path = pathJoin(".", req->path());
    if (req->path().back() == '/') path.append("index.html");

    beast::error_code ec;
    beast::file_stdio fs;
    fs.open(path.c_str(), beast::file_mode::scan, ec);
    vector<uint8_t> data(fs.size(ec));
    fs.read(data.data(), data.size(), ec);

    if (req->method() == CwHttpVerb::head) {
        res->setStatus((int)http::status::ok)
            ->setHeader("Content-Type", string(mimeType(path)))
            ->send();
        return;
    }

    res->setStatus((int)http::status::ok)
        ->setHeader("Content-Type", string(mimeType(path)))
        ->setBody(move(data))
        ->send();
    return;
}
