#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <string_view>

#include "boost-headers.h"

using namespace std;

void fail(CwErrorCode ec, char const* what);
string_view mime_type(string_view path);
string path_cat(string_view base, string_view path);

template<class Body, class Allocator, class Send>
void handle_request(string_view doc_root, CwRequest<Body, CwHttp::basic_fields<Allocator>>&& req, Send&& send) {
    // Returns a bad request response
    auto const bad_request = [&req](string_view why) {
        CwResponse<CwStringBody> res{CwHttp::status::bad_request, req.version()};
        res.set(CwHttp::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(CwHttp::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = string(why);
        res.prepare_payload();
        return res;
    };

    // Returns a not found response
    auto const not_found = [&req](string_view target) {
        CwResponse<CwStringBody> res{CwHttp::status::not_found, req.version()};
        res.set(CwHttp::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(CwHttp::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + string(target) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    // Returns a server error response
    auto const server_error = [&req](string_view what) {
        CwResponse<CwStringBody> res{CwHttp::status::internal_server_error, req.version()};
        res.set(CwHttp::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(CwHttp::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + string(what) + "'";
        res.prepare_payload();
        return res;
    };

    // Make sure we can handle the method
    if (req.method() != CwHttp::verb::get && req.method() != CwHttp::verb::head)
        return send(bad_request("Unknown HTTP-method"));

    // Request path must be absolute and not contain "..".
    if (req.target().empty() || req.target()[0] != '/' || req.target().find("..") != string_view::npos)
        return send(bad_request("Illegal request-target"));

    // Build the path to the requested file
    string path = path_cat(doc_root, req.target());
    if (req.target().back() == '/') path.append("index.html");

    // Attempt to open the file
    CwErrorCode ec;
    CwHttp::file_body::value_type body;
    body.open(path.c_str(), CwBeast::file_mode::scan, ec);

    // Handle the case where the file doesn't exist
    if (ec == CwBeast::errc::no_such_file_or_directory)
        return send(not_found(req.target()));

    // Handle an unknown error
    if (ec)
        return send(server_error(ec.message()));

    // Cache the size since we need it after the move
    auto const size = body.size();

    // Respond to HEAD request
    if (req.method() == CwHttp::verb::head) {
        CwResponse<CwEmptyBody> res{CwHttp::status::ok, req.version()};
        res.set(CwHttp::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(CwHttp::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return send(move(res));
    }

    // Respond to GET request
    CwResponse<CwFileBody> res{
        piecewise_construct,
        make_tuple(move(body)),
        make_tuple(CwHttp::status::ok, req.version())};
    res.set(CwHttp::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(CwHttp::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return send(move(res));
}

#endif // UTILS_H
