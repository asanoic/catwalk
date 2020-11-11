#ifndef CWROUTERDATA_H
#define CWROUTERDATA_H

#include "CwCatwalk.h"
#include "CwObjectData.h"
#include "CwRouter.h"

class CwRequestData;
struct CwRouteTuple {
    vector<string_view> tokenizedPath;
    CwHttpVerb method;
    CwFullHandler handler;

    CwRouteTuple(vector<string_view>&& tokenizedPath, CwHttpVerb method, CwFullHandler handler) :
        tokenizedPath(move(tokenizedPath)),
        method(method),
        handler(handler) {
    }

    CwRouteTuple(CwRouteTuple&& o) :
        tokenizedPath(move(o.tokenizedPath)),
        method(move(o.method)),
        handler(move(o.handler)) {
    }
};

struct CwRouterData : CwObjectData {
    vector<CwRouteTuple> list;

    vector<string_view> tokenize(const string& path);
    int matched(const CwRouteTuple& tuple, CwRequest* req);
    void handler(CwRequest* req, CwResponse* res, CwNextFunc next);
    void action(vector<CwRouteTuple>::const_iterator it, CwRequest* req, CwResponse* res);
};

#endif // CWROUTERDATA_H
