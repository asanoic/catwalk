#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "listener.h"

using namespace std;

using CwIoContext = boost::asio::io_context;
using CwEndPoint = boost::asio::ip::tcp::endpoint;
using CwSignalSet = boost::asio::signal_set;
using CwAddress = boost::asio::ip::address;

int main(int argc, char* argv[]) {

    if (argc != 3) {
        cerr << "Usage: catwalk <port> <doc_root>" << endl
             << "Example:" << endl
             << "    catwalk 8080 ." << endl;
        return EXIT_FAILURE;
    }

    auto const port = atoi(argv[1]);
    auto const doc_root = make_shared<string>(argv[2]);
    auto const threads = max<int>(1, thread::hardware_concurrency());

    CwIoContext ioc(threads);

    make_shared<listener>(ioc, CwEndPoint(CwAddress(), port), doc_root)->run();

    CwSignalSet signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(bind(&CwIoContext::stop, &ioc));

    vector<thread> v(threads - 1);
    v.reserve(threads - 1);
    for (auto i = threads - 1; i > 0; --i)
        v.emplace_back(&CwIoContext::run, &ioc);
    ioc.run();

    // If we get here, it means we got a SIGINT or SIGTERM
    for (auto& t : v) t.join();

    return EXIT_SUCCESS;
}
