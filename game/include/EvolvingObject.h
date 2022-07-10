#ifndef EVOLVINGOBJECT_H
#define EVOLVINGOBJECT_H
#include "DynamicObject.h"

class EvolvingObject: public DynamicObject {
public:
    virtual float getProgress() const =0;
};


#endif // EVOLVINGOBJECT_H
