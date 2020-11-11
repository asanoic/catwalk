#ifndef CWAPPLICATIONDATA_H
#define CWAPPLICATIONDATA_H

#include <functional>

#include "CwRouterData.h"

class CwApplication;
struct CwApplicationData : CwRouterData {
    int threads;
};

#endif // CWAPPLICATIONDATA_H
