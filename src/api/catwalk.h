#ifndef CATWALK_H
#define CATWALK_H

#include <any>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

template<typename T>
using CwSpan = pair<typename T::iterator, typename T::iterator>;

template<typename T>
using CwConstSpan = pair<typename T::const_iterator, typename T::const_iterator>;

using CwBody = vector<uint8_t>;

enum class CwHttpVerb : int {
    none = 0,
    delete_,
    get,
    head,
    post,
    put,
    connect,
    options,
    trace,
};

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
    const CwHttpVerb method() const noexcept;
    vector<any>& data() const noexcept;
};

struct CwResponse {
    CwResponse* setBody(CwBody&& body) noexcept;
    CwResponse* setHeader(string header, string content) noexcept;
    CwResponse* setStatus(int code) noexcept;
    void send() noexcept;
    vector<any>& data() const noexcept;
};

using CwHandler = function<void(CwRequest*, CwResponse*)>;
using CwNextFunc = function<void(void)>;
using CwFullHandler = function<void(CwRequest*, CwResponse*, CwNextFunc)>;

struct CwRouter {
    static CwRouter* newRouter();
    virtual ~CwRouter() noexcept;

    virtual CwRouter* use(string const& path, CwFullHandler handler) noexcept = 0;
    virtual CwRouter* use(string const& path, CwRouter* router) noexcept = 0;
    virtual CwRouter* use(CwRouter* router) noexcept = 0;
    virtual CwRouter* set(CwHttpVerb method, string const& path, CwFullHandler handler) noexcept = 0;
    virtual CwRouter* set(CwHttpVerb method, string const& path, CwHandler handler) noexcept = 0;
};

struct CwApplication : CwRouter {
    void start(uint16_t port) noexcept;
};

#endif // CATWALK_H
