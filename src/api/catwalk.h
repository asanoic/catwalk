#ifndef CATWALK_H
#define CATWALK_H

#include <any>
#include <string_view>
#include <vector>

using namespace std;

template<typename T>
using CwSpan = pair<typename T::iterator, typename T::iterator>;

template<typename T>
using CwConstSpan = pair<typename T::const_iterator, typename T::const_iterator>;

using CwBody = vector<uint8_t>;

struct CwRequest {
    const vector<string_view> headers(string_view header) const noexcept;
    CwConstSpan<CwBody> body() const noexcept;
    const string_view param(string_view name) const noexcept;
    const string_view query(string_view name) const noexcept;
    const string_view get(string_view header) const noexcept;
    const vector<string_view> headers() const noexcept;
    const string_view originalUrl() const noexcept;
    const string_view baseUrl() const noexcept;
    const string_view path() const noexcept;
    vector<any>& data() const noexcept;
};

struct CwResponse {
    CwResponse* setBody(CwBody&& body) noexcept;
    CwResponse* setHeader(string header, string content) noexcept;
    CwResponse* setStatus(int code) noexcept;
    void send() noexcept;
    vector<any>& data() const noexcept;

};

#endif // CATWALK_H
