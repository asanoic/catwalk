#include "CwResponse.h"
#include "CwResponseData.h"

CW_OBJECT_CONSTRUCTOR(CwResponse, CwObject) {
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

vector<any>& CwResponse::data() const noexcept {
    CW_GET_DATA(CwResponse);
    return d->extraData;
}
