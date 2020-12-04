#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <string_view>
#include <iostream>

#include "boost-headers.h"

using namespace std;

const string kServerName = "Catwalk";

void fail(beast::error_code ec, char const* what, char* file, int line);
vector<uint8_t> fromString(string s);

template <typename Itr>
vector<string_view> tokenize(Itr a, Itr b);


#endif // UTILS_H
