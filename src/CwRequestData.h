#ifndef CWREQUESTDATA_H
#define CWREQUESTDATA_H

#include <optional>
using namespace std;

#include "CwObjectData.h"
#include "CwCatwalk.h"
#include "engine/boost-headers.h"

class CwRequest;
struct CwRequestData : CwObjectData {
    optional<bx_request_parser> beastRequestParser;
    pair<CwBody, bool> bodyResult;
    pair<vector<string_view>, bool> headersResult;
    vector<any> extraData;
    unordered_map<string_view, string_view> param;
    unordered_map<string_view, string_view> query;

    void prepareData();
    vector<string_view> preparedPath;
    vector<string_view>::const_iterator pathPos;
};

#endif // CWREQUESTDATA_H

