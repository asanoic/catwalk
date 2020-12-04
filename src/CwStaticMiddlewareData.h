#ifndef CWSTATICMIDDLEWAREDATA_H
#define CWSTATICMIDDLEWAREDATA_H

#include "CwRouterData.h"

class CwStaticMiddleware;
struct CwStaticMiddlewareData : CwRouterData {
    string_view path;
    void handler(CwRequest* req, CwResponse* res, CwNextFunc next);
};


#endif // CWSTATICMIDDLEWAREDATA_H

