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
        bind(&CwApplicationData::handler, d, placeholders::_1, placeholders::_2, CwRouterData::kEmptyCall));

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
