#ifndef CWOBJECTDATA_H
#define CWOBJECTDATA_H

#include <vector>

using namespace std;

class CwObject;

struct CwObjectData {
    virtual ~CwObjectData() {
    }

    CwObject* object;
    CwObject* parent;
    vector<CwObject*> children;
};

#define CW_UNUSED(VAR) \
    ((void*)(VAR))

#define CW_GET_DATAEX(VAR, CLASSNAME, OBJECT) \
    CLASSNAME##Data* const VAR = dynamic_cast<CLASSNAME##Data*>(OBJECT->_data)

#define CW_GET_DATA(CLASSNAME) \
    CW_GET_DATAEX(d, CLASSNAME, this)

#define CW_GET_OBJECTEX(CLASSNAME, OBJECT) \
    CLASSNAME* const o = dynamic_cast<CLASSNAME*>(OBJECT->object)

#define CW_GET_OBJECT(CLASSNAME) \
    CW_GET_OBJECTEX(CLASSNAME, this)

#define CW_OBJECT_CONSTRUCTOR(CLASSNAME, BASENAME)               \
    CLASSNAME::CLASSNAME(CwObjectData* data, CwObject* parent) : \
        BASENAME(data, parent) {                                 \
    }                                                            \
    CLASSNAME::CLASSNAME(CwObject* parent) :                     \
        CLASSNAME(new CLASSNAME##Data, parent)

#endif // CWOBJECTDATA_H
