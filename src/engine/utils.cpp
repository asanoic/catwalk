#include "utils.h"

#include <iostream>
#include <unordered_map>

void fail(beast::error_code ec, char const* what, char* file, int line) {
    cerr << what << ": " << ec.message() << "\n";
    cerr << "line: " << line << " at " << file << "\n";
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
