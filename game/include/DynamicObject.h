#ifndef DYNAMICOBJECT_H
#define DYNAMICOBJECT_H
#include "Team.h"
#include "Positional.h"

enum ObjectType {
    Ship,
    Laser,
    Boom
};

class DynamicObject: public Positional {
public:
    virtual glm::vec2 getVelo() const =0;
    virtual void update(float dt) =0;
    virtual bool isAlive() const =0;
    virtual bool isPointInside(const glm::vec2& pos) const =0;
    virtual bool isTrackable() const =0;
    virtual ObjectType getType() const =0;
    virtual void hit() =0;
    virtual void die() =0;
};

class TeamObject: public DynamicObject {
public:
    TeamObject(Team team);
    virtual ~TeamObject();
    Team getTeam() const;
private:
    Team team;
};


#endif // DYNAMICOBJECT_H
