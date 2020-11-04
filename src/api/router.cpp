#include "catwalk.h"

#include "../lib/path.h"

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
    ~CwRouterImpl() noexcept override;

    CwRouter* use(string const& path, CwFullHandler handler) noexcept override {
        list.emplace_back(tokenize(path), CwHttpVerb::none, handler);
        return this;
    }

    CwRouter* use(string const& path, CwRouter* router) noexcept override {
        list.emplace_back(tokenize(path), CwHttpVerb::none, bind(&CwRouterImpl::handler, router, placeholders::_1, placeholders::_2, placeholders::_3));
        return this;
    }

    CwRouter* use(CwRouter* router) noexcept override {
        list.emplace_back(tokenize(""), CwHttpVerb::none, bind(&CwRouterImpl::handler, router, placeholders::_1, placeholders::_2, placeholders::_3));
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
    vector<string_view> tokenize(string const&) {
        return {};
    }

    bool matched(CwRouteTuple const& tuple, CwConstSpan<string_view> path) {
        return false;
    }

    void handler(CwRequest* req, CwResponse* res, CwNextFunc next) {
        for (auto const& tuple : list) {
            if (!matched(tuple, CwConstSpan<string_view>())) continue;
            tuple.handler(req, res, next);
        }
    }

    void action(vector<CwRouteTuple>::iterator it, CwRequest* req, CwResponse* res) {
        if (it == list.end()) return;
        it->handler(req, res, CwNextFunc());
    }

    void next(vector<CwRouteTuple>::iterator it, CwRequest* req, CwResponse* res) {
        if (matched(*it, CwConstSpan<string_view>())) {
            action(it, req, res);
            ++it;
        }
    }
};

CwRouter* CwRouter::newRouter() {
    return new CwRouterImpl();
}
