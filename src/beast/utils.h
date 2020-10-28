#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <string_view>
#include <iostream>

#include "boost-headers.h"

using namespace std;

void fail(beast::error_code ec, char const* what);
string_view mime_type(string_view path);
string path_cat(string_view base, string_view path);
vector<uint8_t> fromString(string s);

const string kServerName = "Catwalk";

#endif // UTILS_H
