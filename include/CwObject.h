#ifndef CWOBJECT_H
#define CWOBJECT_H

#define CW_OBJECT(CLASSNAME)                                  \
private:                                                      \
    CLASSNAME(const CLASSNAME&) = delete;                     \
    CLASSNAME(CLASSNAME&&) = delete;                          \
    CLASSNAME& operator=(const CLASSNAME&) = delete;          \
    CLASSNAME& operator=(CLASSNAME&&) = delete;               \
                                                              \
protected:                                                    \
    explicit CLASSNAME(CwObjectData* data, CwObject* parent); \
                                                              \
public:                                                       \
    explicit CLASSNAME(CwObject* parent = nullptr);           \
                                                              \
private:

class CwObjectData;
class CwObject {
    CW_OBJECT(CwObject)

public:
    virtual ~CwObject();
    CwObject* parent() const;
    CwObject* setParent(CwObject* parent);

protected:
    CwObjectData* _data;
};

#endif // CwOBJECT_H
