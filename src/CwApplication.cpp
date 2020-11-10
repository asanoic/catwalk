#include "CwApplication.h"
#include "CwApplicationData.h"

#include <iostream>
#include <memory>
#include <thread>
using namespace std;

#include "beast/listener.h"

void __attribute__((constructor)) initForOpenFileLimited() {
#ifdef __MINGW64__
    _setmaxstdio(2048);
#endif
}

CW_OBJECT_CONSTRUCTOR(CwApplication, CwRouter) {
}

int CwApplication::start(uint16_t port) noexcept {
    CW_GET_DATA(CwApplication);

    auto const threads = max<int>(1, thread::hardware_concurrency());

    asio::io_context ioc(threads);

    auto listener = make_unique<CwListener>(
        ioc,
        ip::tcp::endpoint(ip::address(), port),
        bind(&CwApplicationData::handleProc, d, placeholders::_1, placeholders::_2));

    listener->run();

    asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(bind(&asio::io_context::stop, &ioc));

    cout << "port " << port << ", and " << threads << " thread" << (threads > 1 ? "s" : "") << endl;
    vector<thread> v;
    v.reserve(threads - 1);
    for (auto i = threads - 1; i > 0; --i)
        v.emplace_back(&asio::io_context::run, &ioc);
    ioc.run();

    // If we get here, it means we got a SIGINT or SIGTERM
    for (auto& t : v) t.join();

    return EXIT_SUCCESS;
}

void CwApplicationData::handleProc(CwRequest* req, CwResponse* res) {
}
