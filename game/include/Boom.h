#ifndef BOOM_H
#define BOOM_H
#include "EvolvingObject.h"

class BoomObject: public EvolvingObject
{
public:
    BoomObject(const glm::vec2& size,
         const glm::vec2& pos,
         const glm::vec2& velocity,
         float ttl);
    virtual ~BoomObject();
    void update(float dt) override;
    bool isAlive() const override;
    bool isPointInside(const glm::vec2& pos) const override;
    glm::vec2 getPos() const override;
    glm::vec2 getVelo() const override;
    float getRotation() const override;
    glm::vec2 getSize() const override;
    void die() override;
    void hit() override;
    bool isTrackable() const override;
    ObjectType getType() const override;
    float getProgress() const override;
private:
    glm::vec2 size, pos, velocity;
    float rotation, timeToLive,  liefspan;
};


#endif // BOOM_H
