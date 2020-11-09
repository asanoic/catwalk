#ifndef CWAPPLICATIONDATA_H
#define CWAPPLICATIONDATA_H

#include <functional>

#include "CwRouterData.h"

class CwApplication;
struct CwApplicationData : CwRouterData {
    void handleProc(CwRequest* req, CwResponse* res);
};

#endif // CWAPPLICATIONDATA_H
