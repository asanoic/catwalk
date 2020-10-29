#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <string_view>
#include <iostream>

#include "boost-headers.h"

using namespace std;

void fail(beast::error_code ec, char const* what);
string_view mimeType(string_view path);
string pathJoin(string_view base, string_view path);
vector<uint8_t> fromString(string s);

const string kServerName = "Catwalk";

#endif // UTILS_H
