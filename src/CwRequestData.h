#ifndef CWREQUESTDATA_H
#define CWREQUESTDATA_H

#include "CwObjectData.h"
#include "CwCatwalk.h"
#include "beast/boost-headers.h"

class CwRequest;
struct CwRequestData : CwObjectData {
    bx_request_parser beastRequestParser;
    pair<CwBody, bool> bodyResult;
    pair<vector<string_view>, bool> headersResult;
    vector<any> extraData;
};

#endif // CWREQUESTDATA_H

