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
    return this;
}

CwRouter* CwRouter::use(CwRouter* router) noexcept {
    CW_GET_DATA(CwRouter);
    CW_GET_DATAEX(rd, CwRouter, router);
    d->list.emplace_back(d->tokenize(""), CwHttpVerb::none, bind(&CwRouterData::handler, rd, placeholders::_1, placeholders::_2, placeholders::_3));
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

vector<string_view> CwRouterData::tokenize(const string& path) {
    return ::tokenize(path.cbegin(), path.cend());
}

int CwRouterData::matched(const CwRouteTuple& tuple, CwRequest* req) {
    CW_GET_DATAEX(dq, CwRequest, req);
    cout << tuple.tokenizedPath.size() << ", " << dq->preparedPath.size() << endl;
    auto index = mismatch(dq->pathPos, dq->preparedPath.cend(), tuple.tokenizedPath.cbegin(), tuple.tokenizedPath.cend(), [](string_view a, string_view b) {
        if (a == b) return true;
        return a.front() == ':';
    });
    return distance(index.first, dq->preparedPath.cend());
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
    if (int matchLength = matched(*it, req); matchLength > 0) {
        if (it->method == CwHttpVerb::none) {
            vector<string_view>::const_iterator oldPath = exchange(dq->pathPos, dq->pathPos + matchLength);
            it->handler(req, res, next);
            dq->pathPos = oldPath;
        } else if (matchLength == dq->preparedPath.cend() - dq->pathPos) {
            it->handler(req, res, next);
        } else {
            next();
        }
    } else {
        next();
    }
}
