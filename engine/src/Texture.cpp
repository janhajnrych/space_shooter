#include "Texture.h"
#include <iostream>
#include "SOIL.h"

struct Texture::TextureImpl {
    GLuint id = 0;
    glm::ivec2 size = glm::ivec2(0, 0);

    enum State_t {
        NotLoaded,
        LoadingFailed,
        Loaded
    };
    State_t state = NotLoaded;

    void loadImageData(unsigned char* imageData, const glm::vec2& size) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     static_cast<GLsizei>(size.x),
                     static_cast<GLsizei>(size.y),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);
        state = Loaded;
    }

    void setWrapping() {
        // set the texture wrapping parameters
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
       // set texture filtering parameters
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }

};

Texture::Texture(): impl(new TextureImpl()) {}

Texture::Texture(unsigned char* imageData, const glm::ivec2& size): Texture() {
    impl->state = TextureImpl::State_t::NotLoaded;
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    impl->setWrapping();
    impl->loadImageData(imageData, size);
    if (impl->state == TextureImpl::State_t::Loaded){
        impl->size = size;
        impl->id = textureId;
    }
}

std::unique_ptr<Texture> Texture::loadFromPath(const std::string& filePath) {
    int width, height, nrChannels;
    unsigned char* imageData = SOIL_load_image(filePath.c_str(), &width, &height, &nrChannels, SOIL_LOAD_RGBA);
    if (!imageData){
        std::cerr << "cannot load image data from " << filePath << std::endl;
        return nullptr;
    }
    glm::ivec2 size(width, height);
    auto texture = std::make_unique<Texture>(imageData, size);
    SOIL_free_image_data(imageData);
    if (texture->isValid())
        return texture;
    std::cerr << "texture from " << filePath << " coul not be loaded" << std::endl;
    return nullptr;
}

bool Texture::isValid() const {
    return impl->state == TextureImpl::State_t::Loaded;
}

Texture::~Texture() {
    delete impl;
}

void Texture::activate() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, impl->id);
}
