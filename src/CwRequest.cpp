#include "CwRequest.h"
#include "CwRequestData.h"

CW_OBJECT_CONSTRUCTOR(CwRequest, CwObject) {
}

const vector<string_view> CwRequest::headers(string_view header) const noexcept {
    return vector<string_view>{};
}

CwConstSpan<CwBody> CwRequest::body() const noexcept {
    CW_GET_DATA(CwRequest);
    if (!d->bodyResult.second) {
        d->bodyResult.first = d->beastRequestParser.get().body();
        d->bodyResult.second = true;
    }
    return make_pair(d->bodyResult.first.cbegin(), d->bodyResult.first.cend());
}

const string_view CwRequest::param(string_view name) const noexcept {
    return "";
}

const string_view CwRequest::query(string_view name) const noexcept {
    return "";
}

const string_view CwRequest::get(string_view header) const noexcept {
    CW_GET_DATA(CwRequest);
    return d->beastRequestParser.get().base().at(header);
}

const vector<string_view>& CwRequest::headers() const noexcept {
    CW_GET_DATA(CwRequest);
    if (!d->headersResult.second) {
        bx_request::header_type header = d->beastRequestParser.get();
        transform(header.begin(), header.end(), back_inserter(d->headersResult.first), bind(&http::fields::value_type::name_string, placeholders::_1));
        d->headersResult.second = true;
    }
    return d->headersResult.first;
}

const string_view CwRequest::path() const noexcept {
    CW_GET_DATA(CwRequest);
    return d->beastRequestParser.get().target();
}

const CwHttpVerb CwRequest::method() const noexcept {
    CW_GET_DATA(CwRequest);
    return static_cast<CwHttpVerb>(d->beastRequestParser.get().method());
}

vector<any>& CwRequest::data() const noexcept {
    CW_GET_DATA(CwRequest);
    return d->extraData;
}
