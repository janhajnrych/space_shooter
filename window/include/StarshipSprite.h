#ifndef STARSHIPSPRITE_H
#define STARSHIPSPRITE_H
#include "Sprite.h"
#include "Events.h"

class SpriteType;
struct Player;

struct FireEvent {
    float ttl;
    glm::vec2 pos, velo;
};

class StarshipSprite: public DynamicSprite
{
public:
    StarshipSprite(std::shared_ptr<Shader> shader,
                   std::shared_ptr<Texture> texture,
                   glm::vec2 size,
                   const glm::vec2& pos,
                   float rotation);
    void update(float dt) override;
    void draw(Renderer& renderer) override;
    bool isPointInside(const glm::vec2& pos) const override;
    void setSteeringTorque(float torque);
    void setLinearThrust(float thrust);
    void setFireRate(float value);
    glm::vec2 getPos() const override;
    glm::vec2 getVelo() const override;
    Observable<FireEvent>& getWeapon();
    constexpr static float maxThrust = 4.0f;
    constexpr static float maxSpeed = 4.0f;
    constexpr static float maxTorque = 1.5f * 360.0f;
    constexpr static float maxFireRate = 3.0f;
    bool isAlive() const override;
    void die();
    void hit();
    std::string serialize() const;
private:
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> texture;
    glm::vec2 size, pos, velocity, thrust;
    float rotation, angularSpeed, torque, fireCounter, fireRate;
    EventEmitter<FireEvent> projectileEmitter;
    int life;
    FireEvent createProjectile(float angle) const;
};


#endif // STARSHIPSPRITE_H
