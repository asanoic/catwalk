#ifndef CWROUTERDATA_H
#define CWROUTERDATA_H

#include <unordered_set>
using namespace std;

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
    static void kEmptyCall() {};

    vector<CwRouteTuple> list;

    ~CwRouterData();
    vector<string_view> tokenize(const string& path);
    virtual void handler(CwRequest* req, CwResponse* res, CwNextFunc next);
    void action(vector<CwRouteTuple>::const_iterator it, CwRequest* req, CwResponse* res, CwNextFunc uplevelNext);
};

#endif // CWROUTERDATA_H
