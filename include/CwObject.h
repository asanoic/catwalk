#ifndef CWOBJECT_H
#define CWOBJECT_H

#define CW_OBJECT(CLASSNAME)                         \
protected:                                           \
    explicit CLASSNAME(CwObjectData* data);          \
                                                     \
public:                                              \
    explicit CLASSNAME();                            \
    CLASSNAME(const CLASSNAME&) = delete;            \
    CLASSNAME& operator=(const CLASSNAME&) = delete; \
    CLASSNAME& operator=(CLASSNAME&&) = delete;      \
                                                     \
private:

class CwObjectData;
class CwObject {
    CW_OBJECT(CwObject)

public:
    virtual ~CwObject();

protected:
    CwObjectData* _data;
};

#endif // CwOBJECT_H
