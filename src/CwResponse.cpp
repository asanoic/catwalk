#include "CwResponse.h"
#include "CwResponseData.h"

CW_OBJECT_CONSTRUCTOR(CwResponse, CwObject) {
    CW_GET_DATA(CwResponse);
    d->sent = false;
}

CwResponse* CwResponse::setBody(CwBody&& body) noexcept {
    CW_GET_DATA(CwResponse);
    d->beastResponse.body() = move(body);
    return this;
}

CwResponse* CwResponse::setHeader(string header, string content) noexcept {
    CW_GET_DATA(CwResponse);
    d->beastResponse.set(header, content);
    return this;
}

CwResponse* CwResponse::setStatus(int code) noexcept {
    CW_GET_DATA(CwResponse);
    d->beastResponse.result(code);
    return this;
}

void CwResponse::send() noexcept {
    CW_GET_DATA(CwResponse);
    d->sent = true;
}

CwResponse* CwResponse::putExtra(string_view key, any value) noexcept {
    CW_GET_DATA(CwResponse);
    if (value.has_value())
        d->extraData[key] = value;
    else
        d->extraData.erase(key);
    return this;
}

const any& CwResponse::getExtra(string_view key) const noexcept {
    CW_GET_DATA(CwResponse);
    static any defaultValue;
    if (auto p = d->extraData.find(key); p != d->extraData.end()) return p->second;
    return defaultValue;
}
