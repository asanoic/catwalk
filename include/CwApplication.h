#ifndef CWAPPLICATION_H
#define CWAPPLICATION_H

#include "CwRouter.h"

class CwApplicationData;
class CwApplication : public CwRouter {
    CW_OBJECT(CwApplication)

public:
    int start(uint16_t port) noexcept;
};

#endif // CWAPPLICATION_H
