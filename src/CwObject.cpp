#include "CwObject.h"
#include "CwObjectData.h"

#include <algorithm>
using namespace std;

CwObject::CwObject(CwObjectData* data, CwObject* parent) :
    _data(data) {
    CW_GET_DATA(CwObject);
    d->object = this;
    d->parent = parent;
    if (d->parent) {
        auto& children = d->parent->_data->children;
        children.push_back(this);
    }
}

CwObject::CwObject(CwObject* parent) :
    CwObject(new CwObjectData, parent) {
}

CwObject::~CwObject() {
    CW_GET_DATA(CwObject);
    if (d->parent) {
        vector<CwObject*>& children = d->parent->_data->children;
        vector<CwObject*>::iterator p = find(children.begin(), children.end(), this);
        swap(*p, children.back());
        children.pop_back();
    }
    for (CwObject* child : d->children) {
        child->_data->parent = nullptr;
        delete child;
    }
    d->children.clear();
    delete d;
}

CwObject* CwObject::parent() const {
    CW_GET_DATA(CwObject);
    return d->parent;
}

CwObject* CwObject::setParent(CwObject* parent) {
    CW_GET_DATA(CwObject);
    if (d->parent == parent) return this;
    if (d->parent != nullptr) {
        vector<CwObject*>& children = d->parent->_data->children;
        vector<CwObject*>::iterator p = find(children.begin(), children.end(), this);
        swap(*p, children.back());
        children.pop_back();
    }
    if (d->parent = parent; d->parent)
        d->parent->_data->children.push_back(this);
    return this;
}
