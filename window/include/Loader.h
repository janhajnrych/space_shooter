#ifndef LOADER_H
#define LOADER_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "Sprite.h"

class StarshipSprite;
class LaserSprite;
class Background;
class SpriteShader;
class Animation;

class SpriteFactory {
public:
    enum Sprite_t {
        xWing_s,
        tie_s
    };
   virtual std::unique_ptr<StarshipSprite> createStarship(Sprite_t type, const glm::vec2& pos, float rot=0) const =0;
   virtual std::unique_ptr<LaserSprite> createLaser(Sprite_t type) const =0;
   virtual std::unique_ptr<BackgroundTile> createBackground(float scale) const =0;
   virtual std::unique_ptr<Animation> createExplosion(const glm::vec2& pos, const glm::vec2& velo) const =0;

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
