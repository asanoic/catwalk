#include "CwRequest.h"
#include "CwRequestData.h"

#include "utils.h"

CW_OBJECT_CONSTRUCTOR(CwRequest, CwObject) {
}

CwConstSpan<CwBody> CwRequest::body() const noexcept {
    CW_GET_DATA(CwRequest);
    if (!d->bodyResult.second) {
        d->bodyResult.first = d->beastRequestParser->get().body();
        d->bodyResult.second = true;
    }
    return make_pair(d->bodyResult.first.cbegin(), d->bodyResult.first.cend());
}

const string_view CwRequest::param(string_view name) const noexcept {
    return "";
}

optional<reference_wrapper<const vector<string_view>>> CwRequest::query(string_view name) const noexcept {
    CW_GET_DATA(CwRequest);
    optional<reference_wrapper<const vector<string_view>>> ret;
    if (auto p = d->query.find(name); p != d->query.end()) {
        ret = cref(p->second);
    }
    return ret;
}

const string_view CwRequest::get(string_view header) const noexcept {
    CW_GET_DATA(CwRequest);
    return d->beastRequestParser->get().base().at(header);
}

const vector<string_view>& CwRequest::headers() const noexcept {
    CW_GET_DATA(CwRequest);
    if (!d->headersResult.second) {
        bx_request::header_type header = d->beastRequestParser->get();
        transform(header.begin(), header.end(), back_inserter(d->headersResult.first), bind(&http::fields::value_type::name_string, placeholders::_1));
        d->headersResult.second = true;
    }
    return d->headersResult.first;
}

const string_view CwRequest::path() const noexcept {
    CW_GET_DATA(CwRequest);
    return d->path;
}

const CwHttpVerb CwRequest::method() const noexcept {
    CW_GET_DATA(CwRequest);
    return static_cast<CwHttpVerb>(d->beastRequestParser->get().method());
}

vector<any>& CwRequest::data() const noexcept {
    CW_GET_DATA(CwRequest);
    return d->extraData;
}

void CwRequestData::prepareData() {
    target = beastRequestParser->get().target();
    string_view::iterator delimiter = find(target.begin(), target.end(), '?');
    path = string_view(target.begin(), distance(target.begin(), delimiter));
    preparedPath = ::tokenize(path.cbegin(), path.cend());
    pathPos = preparedPath.cbegin();

    static string split = "=&;";

    while (delimiter != target.end()) {
        string_view::iterator left = ++delimiter;
        delimiter = find_first_of(left, target.end(), split.begin(), split.end());
        string_view key(left, distance(left, delimiter));
        if (*delimiter != '=') {
            query[key]; // if not have key, insert key with empty vector of values; if has key, nothing happen;
            continue;
        }
        left = ++delimiter;
        delimiter = find_first_of(left, target.end(), split.begin() + 1, split.end());
        string_view value(left, distance(left, delimiter));
        query[key].push_back(value);
    }
}

vector<string_view>::const_iterator CwRequestData::tokenMatchedUtil(const vector<string_view>& tokens) {
    return mismatch(pathPos, preparedPath.cend(), tokens.cbegin(), tokens.cend(), [](string_view segment, string_view token) {
               if (segment == token) return true;
               return token.front() == ':';
           })
        .second;
}

vector<string_view> CwRequestData::addMatchedParams(const vector<string_view>& tokens) {
    vector<string_view> ret;
    for (int i = 0, n = tokens.size(); i < n; ++i) {
        if (tokens[i].front() == ':') {
            ret.push_back(tokens[i]);
            param.emplace(ret.back(), *(pathPos + i));
        }
    }
    return move(ret);
}
