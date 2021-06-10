#ifndef CWOBJECTDATA_H
#define CWOBJECTDATA_H

class CwObject;

struct CwObjectData {
    virtual ~CwObjectData() {
    }

    CwObject* object;

    static CwObjectData* getData(const CwObject* obj);
};

#define CW_UNUSED(VAR) \
    ((void*)(VAR))

#define CW_GET_DATAEX(VAR, CLASSNAME, OBJECT) \
    CLASSNAME##Data* const VAR = dynamic_cast<CLASSNAME##Data*>(CLASSNAME##Data::getData(OBJECT))

#define CW_GET_DATA(CLASSNAME) \
    CW_GET_DATAEX(d, CLASSNAME, this)

#define CW_GET_OBJECTEX(VAR, CLASSNAME, OBJECT_DATA) \
    CLASSNAME* const VAR = dynamic_cast<CLASSNAME*>(OBJECT_DATA->object)

#define CW_GET_OBJECT(CLASSNAME) \
    CW_GET_OBJECTEX(o, CLASSNAME, this)

#define CW_OBJECT_CONSTRUCTOR(CLASSNAME, BASENAME) \
    CLASSNAME::CLASSNAME(CwObjectData* data) :     \
        BASENAME(data) {                           \
    }                                              \
    CLASSNAME::CLASSNAME() :                       \
        CLASSNAME(new CLASSNAME##Data)

#endif // CWOBJECTDATA_H
