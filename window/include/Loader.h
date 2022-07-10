#ifndef LOADER_H
#define LOADER_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "Sprite.h"
#include "Team.h"

class DynamicObject;
class TeamObject;
class AbstractObjectSprite;
class EvolvingObject;
class Background;
class SpriteShader;
class Animation;
class StarshipObject;
class LaserObject;

class SpriteFactory {
public:
   virtual std::unique_ptr<BackgroundTile> createBackground(float scale) const =0;
   virtual std::shared_ptr<Animation> createExplosion(const glm::vec2& pos, const glm::vec2& velo) const =0;
   virtual std::shared_ptr<AbstractObjectSprite> createShipSprite(std::shared_ptr<StarshipObject> ship) const =0;
   virtual std::unique_ptr<AbstractObjectSprite> createLaserSprite(std::shared_ptr<TeamObject> laser) const =0;
};

class Loader
{
public:
    Loader();
    ~Loader();
    bool load();
    std::shared_ptr<SpriteShader> getSpriteShader() const;
    std::unique_ptr<SpriteFactory> createSpriteFactory() const;
private:
    struct LoaderImpl;
    LoaderImpl* impl;
};




#endif // LOADER_H
