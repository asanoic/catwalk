#ifndef CWRESPONSEDATA_H
#define CWRESPONSEDATA_H

#include "CwObjectData.h"
#include "beast/boost-headers.h"

class CwResponse;
struct CwResponseData : CwObjectData {
    bx_response beastResponse;
    bool sent;
    vector<any> extraData;
};

#endif // CWRESPONSEDATA_H
