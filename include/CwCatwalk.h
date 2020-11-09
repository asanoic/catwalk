#ifndef CWCATWALK_H
#define CWCATWALK_H

#include <vector>
#include <cstdint>

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

#endif // CWCATWALK_H
