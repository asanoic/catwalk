#include "CwRouter.h"
#include "CwRouterData.h"

#include <algorithm>
#include <iostream>
using namespace std;

#include "CwRequestData.h"
#include "utils.h"

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
    d->sub.insert(router);
    return this;
}

CwRouter* CwRouter::use(CwFullHandler handler) noexcept {
    CW_GET_DATA(CwRouter);
    d->list.emplace_back(d->tokenize(""), CwHttpVerb::none, handler);
    return this;
}

CwRouter* CwRouter::use(CwRouter* router) noexcept {
    CW_GET_DATA(CwRouter);
    CW_GET_DATAEX(rd, CwRouter, router);
    d->list.emplace_back(d->tokenize(""), CwHttpVerb::none, bind(&CwRouterData::handler, rd, placeholders::_1, placeholders::_2, placeholders::_3));
    d->sub.insert(router);
    return this;
}

CwRouter* CwRouter::set(CwHttpVerb method, const string& path, CwHandler handler) noexcept {
    if (method == CwHttpVerb::none) {
        cout << "method can not be CwHttpVerb::none" << endl;
        return this;
    }
    CW_GET_DATA(CwRouter);
    d->list.emplace_back(d->tokenize(path), method, bind(handler, placeholders::_1, placeholders::_2));
    return this;
}

CwRouterData::~CwRouterData() {
    for (CwRouter* r : sub) delete r;
}

vector<string_view> CwRouterData::tokenize(const string& path) {
    return ::tokenize(path.cbegin(), path.cend());
}

void CwRouterData::handler(CwRequest* req, CwResponse* res, CwNextFunc next) {
    action(list.cbegin(), req, res);
    if (next) next();
}

void CwRouterData::action(vector<CwRouteTuple>::const_iterator it, CwRequest* req, CwResponse* res) {
    if (it == list.cend()) return;
    CwNextFunc next = [it, req, res, this]() {
        this->action(std::next(it), req, res);
    };
    CW_GET_DATAEX(dq, CwRequest, req);
    auto tuplePos = dq->tokenMatchedUtil(it->tokenizedPath);
    if (tuplePos != it->tokenizedPath.cend()) {
        next();
        return;
    }
    if (it->method == CwHttpVerb::none) {
        vector<string_view> params = dq->addMatchedParams(it->tokenizedPath);
        vector<string_view>::const_iterator oldPathPos = exchange(dq->pathPos, dq->pathPos + it->tokenizedPath.size());
        it->handler(req, res, next);
        dq->pathPos = oldPathPos;
        for (auto& p : params) dq->param.erase(p);
        return;
    }
    if (it->method == req->method() && it->tokenizedPath.size() == distance(dq->pathPos, dq->preparedPath.cend())) {
        vector<string_view> params = dq->addMatchedParams(it->tokenizedPath);
        it->handler(req, res, next);
        for (auto& p : params) dq->param.erase(p);
        return;
    }
    next();
}
