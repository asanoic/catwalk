#ifndef CWREQUESTDATA_H
#define CWREQUESTDATA_H

#include <optional>
using namespace std;

#include "CwCatwalk.h"
#include "CwObjectData.h"
#include "CwRouterData.h"
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
    vector<string_view>::const_iterator tokenMatchedUtil(const vector<string_view>& tokens);
    vector<string_view> addMatchedParams(const vector<string_view>& tokens);
};

#endif // CWREQUESTDATA_H
