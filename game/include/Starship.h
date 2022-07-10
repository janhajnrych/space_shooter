#ifndef STARSHIP_H
#define STARSHIP_H
#include <glm/glm.hpp>
#include "DynamicObject.h"
#include "Team.h"
#include <vector>
#include <functional>
#include <optional>
#include <memory>

class GunObject {
public:
    using Emitter = std::function<void(glm::vec2, glm::vec2)>;
    GunObject(float angle, DynamicObject& parent);
    void fire() const;
    void subscribe(Emitter emitter);
private:
    float angle;
    DynamicObject& parent;
    std::optional<Emitter> emitter;
};

class StarshipObject: public TeamObject
{
public:
    StarshipObject(Team team,
                   const glm::vec2& size,
                   const glm::vec2& pos,
                   float rotation);
    StarshipObject(Team team, size_t playerId);
    virtual ~StarshipObject() override;
    void update(float dt) override;
    bool isPointInside(const glm::vec2& pos) const override;
    void jumpTo(const glm::vec2& pos);
    void setVelo(const glm::vec2& velo);
    void setRot(float rot);
    void setSteeringTorque(float torque);
    void setLinearThrust(float thrust);
    void setFireRate(float value);
    glm::vec2 getPos() const override;
    glm::vec2 getVelo() const override;
    float getRotation() const override;
    glm::vec2 getSize() const override;
    float getFireRate() const;
    constexpr static float maxThrust = 4.0f;
    constexpr static float maxSpeed = 4.0f;
    constexpr static float maxTorque = 1.5f * 360.0f;
    constexpr static float maxFireRate = 3.0f;
    constexpr static float supressedFireRate = 0.0f;
    constexpr static glm::vec2 defaultSize = glm::vec2(0.075f, 0.075f);
    bool isAlive() const override;
    void die() override;
    void hit() override;
    bool isTrackable() const override;
    ObjectType getType() const override;
    static glm::vec2 getDefaultStartPos(size_t id);
    const std::vector<std::unique_ptr<GunObject>>& getGuns() const;
private:
    glm::vec2 size, pos, velocity, thrust;
    float rotation, angularSpeed, torque, fireCounter, fireRate;
    int life;
    std::vector<std::unique_ptr<GunObject>> guns;
};


#endif // STARSHIP_H
