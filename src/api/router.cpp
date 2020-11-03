#include "catwalk.h"

#include "../lib/path.h"

struct CwRouterImpl : CwRouter {
    virtual ~CwRouterImpl() noexcept override;

    virtual CwRouter* use(string path, CwFullHandler handler) noexcept override;
    virtual CwRouter* use(string path, CwRouter* router) noexcept override;
    virtual CwRouter* use(CwRouter* router) noexcept override;
    virtual CwRouter* set(CwHttpVerb method, string path, CwFullHandler handler) noexcept override;
    virtual CwRouter* set(CwHttpVerb method, string path, CwHandler handler) noexcept override;

    vector<pair<CwPath, CwRouterImpl*>> list;
    string currentPath;
};

CwRouter* CwRouter::newRouter() {
    return new CwRouterImpl();
}

CwRouter* CwRouterImpl::use(string path, CwFullHandler handler) noexcept {
    list.emplace_back(CwPath(path, false), nullptr);
    return this;
}

CwRouter* CwRouterImpl::use(string path, CwRouter* router) noexcept {
    list.emplace_back(CwPath(path, false), router);
    return this;
}

CwRouter* CwRouterImpl::use(CwRouter* router) noexcept {
    list.emplace_back(CwPath("", false), router);
    return this;
}

CwRouter* CwRouterImpl::set(CwHttpVerb method, string path, CwFullHandler handler) noexcept {
    return this;
}

CwRouter* CwRouterImpl::set(CwHttpVerb method, string path, CwHandler handler) noexcept {
    return this;
}
