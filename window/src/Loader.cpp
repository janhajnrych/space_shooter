#include "Loader.h"
#include <glad.h>
#include <iostream>
#include <unordered_map>
#include "SpriteShader.h"
#include "Game.h"
#include "Texture.h"
#include <unordered_map>
#include "Animation.h"
#include <tuple>
#include <fstream>
#include <algorithm>
#include "ObjectSprite.h"
#include "EvolvingObject.h"
#include "Starship.h"
#include "Boom.h"
#include <glm/glm.hpp>
#include <filesystem>

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
            textureMap(textureMap), animationMap(animationMap), spriteShader(spriteShader) {
        }

        virtual ~DynamicSpriteFactory() = default;

        std::shared_ptr<AbstractObjectSprite> createShipSprite(std::shared_ptr<StarshipObject> ship) const override {
            auto team = ship->getTeam();
            auto txId = shipTextures.at(team);
            auto texture = textureMap.at(txId);
            return std::make_shared<ColoredObjectSprite>(spriteShader, texture, ship, noColor);
        }

        std::unique_ptr<AbstractObjectSprite> createLaserSprite(std::shared_ptr<TeamObject> laser) const override {
            auto texture = textureMap.at(Texture_t::laser_tx);
            auto team = laser->getTeam();
            return std::make_unique<ColoredObjectSprite>(spriteShader, texture, laser, colors.at(team));
        }

        std::unique_ptr<BackgroundTile> createBackground(float nRepeatBg) const override {
            auto scale = 1.0f/nRepeatBg;
            glm::vec2 size(scale, scale);
            std::shared_ptr<Texture> tx = textureMap.at(space_tx);
            return std::make_unique<BackgroundTile>(tx, spriteShader, size);
        }

        std::shared_ptr<Animation> createExplosion(const glm::vec2& pos, const glm::vec2& velo) const override {
            auto boomObj = std::make_shared<BoomObject>(boomSize, pos, velo, boomTtl);
            return std::make_shared<Animation>(spriteShader, animationMap.at(boom_tx), boomObj);
        }

    private:
        const glm::vec2 laserSize = glm::vec2(0.075f, 0.005f);
        const glm::vec2 boomSize = glm::vec2(0.05f, 0.05f);
        const glm::vec4 noColor = glm::vec4(1,1,1,1);

        const float boomTtl = 0.5f;

        std::unordered_map<Team, glm::vec4> colors = {
            {Team::Blue, glm::vec4(0,0,1,1)},
            {Team::Red, glm::vec4(1,0,0,1)}
        };
        std::unordered_map<Team, Texture_t> shipTextures = {
            {Team::Blue, xWing_tx},
            {Team::Red, tie_tx}
        };
        std::unordered_map<Texture_t, std::shared_ptr<Texture>> textureMap;
        std::unordered_map<Texture_t, std::vector<std::shared_ptr<Texture>>> animationMap;
        std::shared_ptr<SpriteShader> spriteShader;
    };

}

struct Loader::LoaderImpl {

  std::shared_ptr<SpriteShader> spriteShader;
  using TxPtr = std::shared_ptr<Texture>;
  std::unordered_map<Texture_t, TxPtr> textureMap;
  std::unordered_map<Texture_t, std::vector<TxPtr>> animationTextureMap;
  std::vector<std::tuple<std::string, Texture_t>> objectFilenames;

  LoaderImpl() {
      objectFilenames = {
          {"xwing.png", xWing_tx},
          {"tie.png", tie_tx},
          {"space.png", space_tx},
          {"laser.png", laser_tx}
      };
      objectFilenames.shrink_to_fit();
  }

  std::vector<std::shared_ptr<Texture>> loadAlphabeticalyFromDir(const std::filesystem::path& path){
      std::vector<std::tuple<std::string, std::shared_ptr<Texture>>> cache;
      for (const auto& entry : std::filesystem::directory_iterator(path)) {
          std::cout << entry.path().c_str() << std::endl;
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

  bool loadAssets(const std::filesystem::path& dirPath){
      bool success = true;
      for (const auto& entry : objectFilenames){
        const std::filesystem::path filepath = std::get<0>(entry).c_str();
        textureMap[std::get<1>(entry)] = Texture::loadFromPath(dirPath / filepath);
      }
      auto frames = loadAlphabeticalyFromDir(dirPath / std::filesystem::path("boom"));
      animationTextureMap[boom_tx] = std::vector<std::shared_ptr<Texture>>(frames);
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
    auto assetDirPath = std::filesystem::current_path().parent_path() / std::filesystem::path("assets");
    bool valid = impl->loadAssets(assetDirPath);
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



