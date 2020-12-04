#ifndef CWSTATICMIDDLEWARE_H
#define CWSTATICMIDDLEWARE_H

#include "CwRouter.h"

class CwStaticMiddlewareData;
class CwStaticMiddleware : public CwRouter {
    CW_OBJECT(CwStaticMiddleware)

public:
    CwStaticMiddleware(string_view path);

public:
    static CwStaticMiddleware* instance(string_view path);
};

#endif // CWSTATICMIDDLEWARE_H
