#ifndef CWAPPLICATION_H
#define CWAPPLICATION_H

#include "CwRouter.h"

class CwApplicationData;
class CwApplication : public CwRouter {
    CW_OBJECT(CwApplication)

public:
    int start(uint16_t port) noexcept;
    int threads() noexcept;

    void demo(CwRequest* req, CwResponse* res, CwNextFunc next);
};

#endif // CWAPPLICATION_H
