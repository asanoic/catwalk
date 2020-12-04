#include "CwStaticMiddleware.h"
#include "CwStaticMiddlewareData.h"

#include <iostream>
using namespace std;

#include "CwResponseData.h"
#include "utils.h"

// Return a reasonable mime type based on the extension of a file.
string_view mimeType(string_view path) {
    static unordered_map<string_view, string_view> extTable = {
        {".htm", "text/html"},
        {".html", "text/html"},
        {".php", "text/html"},
        {".css", "text/css"},
        {".txt", "text/plain"},
        {".js", "application/javascript"},
        {".json", "application/json"},
        {".xml", "application/xml"},
        {".swf", "application/x-shockwave-flash"},
        {".flv", "video/x-flv"},
        {".png", "image/png"},
        {".jpe", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".jpg", "image/jpeg"},
        {".gif", "image/gif"},
        {".bmp", "image/bmp"},
        {".ico", "image/vnd.microsoft.icon"},
        {".tiff", "image/tiff"},
        {".tif", "image/tiff"},
        {".svg", "image/svg+xml"},
        {".svgz", "image/svg+xml"},
    };
    const size_t pos = path.rfind(".");
    if (pos == string_view::npos) return string_view();
    auto p = extTable.find(path.substr(pos));

    return p == extTable.end() ? "application/text" : p->second;
}

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
string pathJoin(string_view base, string_view path) {
    if (base.empty()) return string(path);
    string result(base);
    char constexpr path_separator = '/';
    if (result.back() == path_separator) result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    return result;
}

CW_OBJECT_CONSTRUCTOR(CwStaticMiddleware, CwRouter) {
}

CwStaticMiddleware::CwStaticMiddleware(string_view path) :
    CwStaticMiddleware() {
    CW_GET_DATA(CwStaticMiddleware);
    d->path = path;
}

CwStaticMiddleware* CwStaticMiddleware::instance(string_view path) {
    static CwStaticMiddleware mdStatic(path);
    return &mdStatic;
}

void CwStaticMiddlewareData::handler(CwRequest* req, CwResponse* res, CwNextFunc next) {
    next();
    CW_GET_DATAEX(resd, CwResponse, res);
    if (resd->sent) return;

    if (req->method() != CwHttpVerb::get && req->method() != CwHttpVerb::head) return;

    // Request path must be absolute and not contain "..".
    if (req->path().empty() || req->path()[0] != '/' || req->path().find("..") != string_view::npos) return;

    string path = pathJoin(this->path, req->path());
    if (req->path().back() == '/') path.append("index.html");

    beast::error_code ec;
    beast::file_stdio fs;
    fs.open(path.c_str(), beast::file_mode::scan, ec);
    if (ec) return;
    vector<uint8_t> data(fs.size(ec));
    fs.read(data.data(), data.size(), ec);

    if (req->method() == CwHttpVerb::head) {
        res->setStatus((int)http::status::ok)
            ->setHeader("Content-Type", string(mimeType(path)))
            ->send();
        return;
    }

    res->setStatus((int)http::status::ok)
        ->setHeader("Content-Type", string(mimeType(path)))
        ->setBody(move(data))
        ->send();
    return;
}
