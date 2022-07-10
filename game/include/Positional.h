#ifndef POSITIONAL_H
#define POSITIONAL_H
#include <glm/glm.hpp>

class Positional {
public:
    virtual glm::vec2 getPos() const =0;
    virtual float getRotation() const =0;
    virtual glm::vec2 getSize() const =0;
};


#endif // POSITIONAL_H
