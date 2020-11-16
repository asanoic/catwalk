#ifndef CWRESPONSE_H
#define CWRESPONSE_H

#include <string>
#include <any>

#include "CwCatwalk.h"
#include "CwObject.h"

class CwResponseData;
class CwResponse : public CwObject {
    CW_OBJECT(CwResponse)
public:
    CwResponse* setBody(CwBody&& body) noexcept;
    CwResponse* setHeader(string header, string content) noexcept;
    CwResponse* setStatus(int code) noexcept;
    void send() noexcept;
    CwResponse* putExtra(string_view key, any value) noexcept;
    const any& getExtra(string_view key) const noexcept;
};

#endif // CWRESPONSE_H
