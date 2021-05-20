#include "Loader.h"
#include <glad.h>
#include <iostream>
#include <unordered_map>
#include "SpriteShader.h"
#include "StarshipSprite.h"
#include "Game.h"
#include "Texture.h"
#include <unordered_map>
#include "LaserSprite.h"
#include <filesystem>
#include "Animation.h"
#include <tuple>
#include <fstream>

namespace  {

    enum Texture_t {
        xWing_tx =0,
        tie_tx =1,
        space_tx=2,
        laser_tx=3,
        boom_tx=4
    };

    class DynamicSpriteFactory: public SpriteFactory {
    public:

        DynamicSpriteFactory(std::unordered_map<Texture_t, std::shared_ptr<Texture>>&& textureMap,
                             std::unordered_map<Texture_t, std::vector<std::shared_ptr<Texture>>>&& animationMap,
                             std::shared_ptr<SpriteShader> spriteShader):
            textureMap(textureMap), animationMap(animationMap), spriteShader(spriteShader){
        }

        virtual ~DynamicSpriteFactory() = default;

        std::unique_ptr<StarshipSprite> createStarship(Sprite_t typeId, const glm::vec2& pos, float rot=0) const override {
            return std::make_unique<StarshipSprite>(spriteShader, textureMap.at(shipTextures.at(typeId)),
                                                    shipSize, pos, rot);
        }

        std::unique_ptr<LaserSprite> createLaser(Sprite_t typeId) const override {
            return std::make_unique<LaserSprite>(spriteShader, textureMap.at(laser_tx),
                                                 laserSize, colors.at(typeId));
        }

        std::unique_ptr<BackgroundTile> createBackground(float nRepeatBg) const override {
            auto scale = 1.0f/nRepeatBg;
            glm::vec2 size(scale, scale);
            std::shared_ptr<Texture> tx = textureMap.at(space_tx);
            return std::make_unique<BackgroundTile>(tx, spriteShader, size);
        }

        std::unique_ptr<Animation> createExplosion(const glm::vec2& pos, const glm::vec2& velo) const override {
            return std::make_unique<Animation>(spriteShader, animationMap.at(boom_tx), boomSize, pos, velo, boomTtl,
                                               glm::atan(velo.y, velo.x));
        }

    private:
        const glm::vec2 shipSize = glm::vec2(0.075f, 0.075f);
        const glm::vec2 laserSize = glm::vec2(0.075f, 0.005f);
        const glm::vec2 boomSize = glm::vec2(0.05f, 0.05f);

        std::unordered_map<Sprite_t, glm::vec4> colors = {
            {xWing_s, glm::vec4(0,0,1,1)},
            {tie_s, glm::vec4(1,0,0,1)}
        };
        std::unordered_map<Sprite_t, Texture_t> shipTextures = {
            {xWing_s, xWing_tx},
            {tie_s, tie_tx}
        };
        std::unordered_map<Texture_t, std::shared_ptr<Texture>> textureMap;
        std::unordered_map<Texture_t, std::vector<std::shared_ptr<Texture>>> animationMap;
        std::shared_ptr<SpriteShader> spriteShader;

        const float boomTtl = 0.5f;
    };

}

struct Loader::LoaderImpl {

  std::shared_ptr<SpriteShader> spriteShader;
  using TxPtr = std::shared_ptr<Texture>;
  std::unordered_map<Texture_t, TxPtr> textureMap;
  std::unordered_map<Texture_t, std::vector<TxPtr>> animationTextureMap;

  LoaderImpl() {}

  std::vector<std::shared_ptr<Texture>> loadAlphabeticalyFromDir(const std::string& path){
      using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
      std::vector<std::tuple<std::string, std::shared_ptr<Texture>>> cache;
      for (const auto& entry : recursive_directory_iterator(path.c_str())){
          std::shared_ptr<Texture> tx = Texture::loadFromPath(entry.path().c_str());
          if (tx == nullptr)
              continue;
          cache.push_back(std::make_tuple(std::string(entry.path().c_str()), tx));
      }
      std::sort(cache.begin(), cache.end(), [](const auto& left, const auto& right){
         return std::get<0>(left) < std::get<0>(right);
      });
      std::vector<std::shared_ptr<Texture>> textures;
      std::transform(cache.begin(), cache.end(), std::back_inserter(textures),
                     [](const auto& item) { return std::get<1>(item); });
      return textures;
  }

  bool loadAssets(const std::string& dirPath){
      bool success = true;
      textureMap[xWing_tx] = Texture::loadFromPath(dirPath + "xwing.png");
      textureMap[tie_tx] = Texture::loadFromPath(dirPath + "tie.png");
      textureMap[space_tx] = Texture::loadFromPath(dirPath + "space.png");
      textureMap[laser_tx] = Texture::loadFromPath(dirPath + "laser.png");
      auto frames = loadAlphabeticalyFromDir(dirPath + "boom");
      animationTextureMap[boom_tx] = std::vector<std::shared_ptr<Texture>>(frames);
//      std::reverse(frames.begin(), frames.end());
//      animationTextureMap[boom_tx].insert(animationTextureMap[boom_tx].end(), frames.begin()+1, frames.end());
      return success;
  }

  bool createShaders() {
      spriteShader = std::make_shared<SpriteShader>();
      if (!spriteShader->isValid()){
          std::cerr << "sprite shader is invalid" << std::endl;
          return false;
      }
      return true;
  }

};

Loader::Loader(): impl(new LoaderImpl()){}

bool Loader::load() {
    std::ifstream configFile("config.txt");
    std::string line;
    std::getline(configFile, line);  // we only need first line which should be asset directory path
    if (line.size() == 0)
        return false;
    bool valid = impl->loadAssets(line);
    valid&= impl->createShaders();
    return valid;
}

Loader::~Loader(){
    delete impl;
}

std::shared_ptr<SpriteShader> Loader::getSpriteShader() const {
    return impl->spriteShader;
}

std::unique_ptr<SpriteFactory> Loader::createSpriteFactory() const {
   std::unique_ptr<DynamicSpriteFactory> factory = std::make_unique<DynamicSpriteFactory>(std::move(impl->textureMap),
                                                                                          std::move(impl->animationTextureMap),
                                                                                          impl->spriteShader);
   return std::move(factory);  // rely on type slicing
}



