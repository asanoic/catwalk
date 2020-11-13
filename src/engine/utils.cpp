#include "utils.h"

#include <iostream>
#include <unordered_map>

void fail(beast::error_code ec, char const* what, char* file, int line) {
    cerr << what << ": " << ec.message() << "\n";
    cerr << "line: " << line << " at " << file << "\n";
}

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

vector<uint8_t> fromString(string s) {
    return vector<uint8_t>(s.begin(), s.end());
}

template <typename Itr>
vector<string_view> tokenize(Itr a, Itr b) {
    Itr pos = a, next = a;
    vector<string_view> ret;
    while (true) {
        pos = next;
        if (pos == b) break;
        while (next != b && *next != '/') ++next;
        if (next != b && next == pos) ++next;
        ret.emplace_back(&*pos, next - pos);
    }
    return ret;
}

template vector<string_view> tokenize(string::const_iterator a, string::const_iterator b);
template vector<string_view> tokenize(string_view::const_iterator a, string_view::const_iterator b);
