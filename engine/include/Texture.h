#ifndef TEXTURE_H
#define TEXTURE_H
#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

class Texture
{
public:
    Texture();
    Texture(unsigned char* imageData, const glm::ivec2& size);
    ~Texture();
    void activate();
    bool isValid() const;
    static std::unique_ptr<Texture> loadFromPath(const std::string& filePath); // factory method
private:
    struct TextureImpl;
    TextureImpl* impl;
};


#endif // TEXTURE_H
