#ifndef CWRESPONSEDATA_H
#define CWRESPONSEDATA_H

#include "CwObjectData.h"
#include "engine/boost-headers.h"

class CwResponse;
struct CwResponseData : CwObjectData {
    bx_response beastResponse;
    bool sent;
    unordered_map<string_view, any> extraData;
};

#endif // CWRESPONSEDATA_H
