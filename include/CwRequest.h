#ifndef CWREQUEST_H
#define CWREQUEST_H

#include <any>
#include <string>

#include "CwCatwalk.h"
#include "CwObject.h"

class CwRequestData;
class CwRequest : public CwObject {
    CW_OBJECT(CwRequest)
public:
    const vector<string_view> headers(string_view header) const noexcept;
    CwConstSpan<CwBody> body() const noexcept;
    const string_view param(string_view name) const noexcept;
    const string_view query(string_view name) const noexcept;
    const string_view get(string_view header) const noexcept;
    const vector<string_view>& headers() const noexcept;
    const string_view path() const noexcept;
    const CwHttpVerb method() const noexcept;
    vector<any>& data() const noexcept;
};

#endif // CWREQUEST_H
