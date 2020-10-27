#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <cstdlib>

#include "listener.h"

using namespace std;

int main(int argc, char* argv[]) {

    if (argc != 3) {
        cerr << "Usage: catwalk <port> <doc_root>" << endl
             << "Example:" << endl
             << "    catwalk 8080 ." << endl;
        return EXIT_FAILURE;
    }

#ifdef __MINGW64__
    _setmaxstdio(2048);
#endif

    auto const port = atoi(argv[1]);
    auto const doc_root = make_shared<string>(argv[2]);
    auto const threads = max<int>(1, thread::hardware_concurrency());

    asio::io_context ioc(threads);

    make_shared<CwListener>(ioc, ip::tcp::endpoint(ip::address(), port), doc_root)->run();

    asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(bind(&asio::io_context::stop, &ioc));

    vector<thread> v(threads - 1);
    v.reserve(threads - 1);
    for (auto i = threads - 1; i > 0; --i)
        v.emplace_back(&asio::io_context::run, &ioc);
    ioc.run();

    // If we get here, it means we got a SIGINT or SIGTERM
    for (auto& t : v) t.join();

    return EXIT_SUCCESS;
}
