#include "CwRouter.h"
#include "CwRouterData.h"

CW_OBJECT_CONSTRUCTOR(CwRouter, CwObject) {
}

CwRouter* CwRouter::use(const string& path, CwFullHandler handler) noexcept {
    CW_GET_DATA(CwRouter);
    d->list.emplace_back(d->tokenize(path), CwHttpVerb::none, handler);
    return this;
}

CwRouter* CwRouter::use(const string& path, CwRouter* router) noexcept {
    CW_GET_DATA(CwRouter);
    CW_GET_DATAEX(rd, CwRouter, router);
    d->list.emplace_back(d->tokenize(path), CwHttpVerb::none, bind(&CwRouterData::handler, rd, placeholders::_1, placeholders::_2, placeholders::_3));
    return this;
}

CwRouter* CwRouter::use(CwRouter* router) noexcept {
    CW_GET_DATA(CwRouter);
    CW_GET_DATAEX(rd, CwRouter, router);
    d->list.emplace_back(d->tokenize(""), CwHttpVerb::none, bind(&CwRouterData::handler, rd, placeholders::_1, placeholders::_2, placeholders::_3));
    return this;
}

CwRouter* CwRouter::set(CwHttpVerb method, const string& path, CwFullHandler handler) noexcept {
    CW_GET_DATA(CwRouter);
    d->list.emplace_back(d->tokenize(path), method, handler);
    return this;
}

CwRouter* CwRouter::set(CwHttpVerb method, const string& path, CwHandler handler) noexcept {
    CW_GET_DATA(CwRouter);
    d->list.emplace_back(d->tokenize(path), method, bind(handler, placeholders::_1, placeholders::_2));
    return this;
}

vector<string_view> CwRouterData::tokenize(const string& path) {
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

bool CwRouterData::matched(const CwRouteTuple& tuple, CwConstSpan<string_view> path) {
    return false;
}

void CwRouterData::handler(CwRequest* req, CwResponse* res, CwNextFunc next) {
    action(list.cbegin(), req, res);
    //TODO: need review here
    next();
}

void CwRouterData::action(vector<CwRouteTuple>::const_iterator it, CwRequest* req, CwResponse* res) {
    if (it == list.end()) return;
    if (!matched(*it, CwConstSpan<string_view>())) return;
    CwNextFunc next = [it, req, res, this]() {
        this->action(std::next(it), req, res);
    };
    it->handler(req, res, next);
}
