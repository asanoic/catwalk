#ifndef CWROUTER_H
#define CWROUTER_H

#include <functional>
#include <string>
using namespace std;

#include "CwCatwalk.h"
#include "CwObject.h"
#include "CwRequest.h"
#include "CwResponse.h"

using CwHandler = function<void(CwRequest*, CwResponse*)>;
using CwNextFunc = function<void(void)>;
using CwFullHandler = function<void(CwRequest*, CwResponse*, CwNextFunc)>;

class CwRouterData;
class CwRouter : public CwObject {
    CW_OBJECT(CwRouter)

public:
    CwRouter* use(const string& path, CwFullHandler handler) noexcept;
    CwRouter* use(const string& path, CwRouter* router) noexcept;
    CwRouter* use(CwRouter* router) noexcept;
    CwRouter* set(CwHttpVerb method, const string& path, CwHandler handler) noexcept;
};

#endif // CWROUTER_H
