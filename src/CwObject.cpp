#include "CwObject.h"
#include "CwObjectData.h"

CwObject::CwObject(CwObjectData* data) :
    _data(data) {
    CW_GET_DATA(CwObject);
    d->object = this;
}

CwObject::CwObject() :
    CwObject(new CwObjectData) {
}

CwObject::~CwObject() {
    CW_GET_DATA(CwObject);
    delete d;
}

CwObjectData* CwObjectData::getData(const CwObject* obj) {
    return obj->_data;
}
