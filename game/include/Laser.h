#ifndef LASER_H
#define LASER_H
#include "DynamicObject.h"

class LaserObject: public TeamObject
{
public:
    LaserObject(Team team,
          const glm::vec2& pos,
          const glm::vec2& velocity,
          float ttl);
    void update(float dt) override;
    bool isAlive() const override;
    bool isPointInside(const glm::vec2& pos) const override;
    glm::vec2 getPos() const override;
    glm::vec2 getVelo() const override;
    void die() override;
    void hit() override;
    bool isTrackable() const override;
    ObjectType getType() const override;
    float getRotation() const override;
    glm::vec2 getSize() const override;
private:
    glm::vec2 pos, velocity;
    float rotation, timeToLive;

};




#endif // LASER_H
