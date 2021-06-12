#include "CwRouter.h"
#include "CwRouterData.h"

#include <algorithm>
#include <iostream>
using namespace std;

#include "CwRequestData.h"
#include "CwResponseData.h"
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
    CW_GET_DATAEX(dRouter, CwRouter, router);
    d->list.emplace_back(d->tokenize(path), CwHttpVerb::none, bind(&CwRouterData::handler, dRouter, placeholders::_1, placeholders::_2, placeholders::_3));
    return this;
}

CwRouter* CwRouter::use(CwFullHandler handler) noexcept {
    CW_GET_DATA(CwRouter);
    d->list.emplace_back(d->tokenize(""), CwHttpVerb::none, handler);
    return this;
}

CwRouter* CwRouter::use(CwRouter* router) noexcept {
    CW_GET_DATA(CwRouter);
    CW_GET_DATAEX(dRouter, CwRouter, router);
    d->list.emplace_back(d->tokenize(""), CwHttpVerb::none, bind(&CwRouterData::handler, dRouter, placeholders::_1, placeholders::_2, placeholders::_3));
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
    CwNextFunc next = bind(&CwRouterData::action, this, std::next(it), req, res);

    CW_GET_DATAEX(dReq, CwRequest, req);
    auto tuplePos = dReq->tokenMatchedUtil(it->tokenizedPath);
    if (tuplePos != it->tokenizedPath.cend()) {
        // not match route, move to next;
        next();

    } else if (it->method == CwHttpVerb::none) {
        // route matched, it is middleware, because handler added by method use
        vector<string_view> params = dReq->addMatchedParams(it->tokenizedPath);
        vector<string_view>::const_iterator oldPathPos = exchange(dReq->pathPos, dReq->pathPos + it->tokenizedPath.size());
        it->handler(req, res, next);
        dReq->pathPos = oldPathPos;
        for (auto& p : params) dReq->param.erase(p);

    } else if (it->method == req->method() && it->tokenizedPath.size() == distance(dReq->pathPos, dReq->preparedPath.cend())) {
        // route matched, and has method, which is added by set, check complete match
        vector<string_view> params = dReq->addMatchedParams(it->tokenizedPath);
        it->handler(req, res, next);
        for (auto& p : params) dReq->param.erase(p);

    } else {
        // not for set, move to next
        next();
    }
}
