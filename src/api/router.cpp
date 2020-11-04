#include "catwalk.h"

#include "../lib/path.h"

#include "../beast/listener.h"

struct CwRouteTuple {
    vector<string_view> tokenizedPath;
    CwHttpVerb method;
    CwFullHandler handler;

    CwRouteTuple(vector<string_view>&& tokenizedPath, CwHttpVerb method, CwFullHandler handler) :
        tokenizedPath(move(tokenizedPath)),
        method(method),
        handler(handler) {
    }

    CwRouteTuple(CwRouteTuple&& o) :
        tokenizedPath(move(o.tokenizedPath)),
        method(move(o.method)),
        handler(move(o.handler)) {
    }
};

struct CwRouterImpl : CwRouter {

    CwRouter* use(string const& path, CwFullHandler handler) noexcept override {
        list.emplace_back(tokenize(path), CwHttpVerb::none, handler);
        return this;
    }

    CwRouter* use(string const& path, CwRouter* router) noexcept override {
        list.emplace_back(tokenize(path), CwHttpVerb::none, bind(&CwRouterImpl::handler, (CwRouterImpl*)router, placeholders::_1, placeholders::_2, placeholders::_3));
        return this;
    }

    CwRouter* use(CwRouter* router) noexcept override {
        list.emplace_back(tokenize(""), CwHttpVerb::none, bind(&CwRouterImpl::handler, (CwRouterImpl*)router, placeholders::_1, placeholders::_2, placeholders::_3));
        return this;
    }

    CwRouter* set(CwHttpVerb method, string const& path, CwFullHandler handler) noexcept override {
        list.emplace_back(tokenize(path), method, handler);
        return this;
    }

    CwRouter* set(CwHttpVerb method, string const& path, CwHandler handler) noexcept override {
        list.emplace_back(tokenize(path), method, bind(handler, placeholders::_1, placeholders::_2));
        return this;
    }

    vector<CwRouteTuple> list;

private:
    vector<string_view> tokenize(string const& path) {
        string::const_iterator pos = path.cbegin(), next = path.cbegin();
        vector<string_view> ret;
        while (true) {
            pos = next;
            if (pos == path.end()) break;
            while (next != path.cend() && *next != '/') ++next;
            if (next != path.cend() && next == pos) ++next;
            ret.emplace_back(&*pos, next - pos);
        }
        return ret;
    }

    bool matched(CwRouteTuple const& tuple, CwConstSpan<string_view> path) {
        return false;
    }

    void handler(CwRequest* req, CwResponse* res, CwNextFunc next) {
        action(list.cbegin(), req, res);
        //TODO: need review here
        next();
    }

    void action(vector<CwRouteTuple>::const_iterator it, CwRequest* req, CwResponse* res) {
        if (it == list.end()) return;
        if (!matched(*it, CwConstSpan<string_view>())) return;
        CwNextFunc next = [it, req, res, this]() {
            this->action(std::next(it), req, res);
        };
        it->handler(req, res, next);
    }
};

struct CwApplicationImpl : CwApplication, CwRouterImpl {
    int start(uint16_t port) noexcept {

        auto const doc_root = make_shared<string>();
        auto const threads = max<int>(1, thread::hardware_concurrency());

        asio::io_context ioc(threads);

        auto listener = make_unique<CwListener>(ioc, ip::tcp::endpoint(ip::address(), port), doc_root);
        listener->run();

        asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait(bind(&asio::io_context::stop, &ioc));

        vector<thread> v;
        v.reserve(threads - 1);
        for (auto i = threads - 1; i > 0; --i)
            v.emplace_back(&asio::io_context::run, &ioc);
        ioc.run();

        // If we get here, it means we got a SIGINT or SIGTERM
        for (auto& t : v) t.join();

        return EXIT_SUCCESS;
    }
};

CwRouter* CwRouter::newRouter() {
    return new CwRouterImpl();
}
