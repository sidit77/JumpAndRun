#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <ttvfs.h>
#include "../../util/filesystem.h"

using namespace jnr;

Texture::Texture() {
    glGenTextures(1, &id);
}

Texture::Texture(const std::string& path) : Texture() {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int size, width, height, nrChannels;
    unsigned char *buffer, *data;
    if((buffer = (unsigned char *) services::filesystem->readAllBytes(path, &size)) &&
       (data = stbi_load_from_memory(buffer, size, &width, &height, &nrChannels, STBI_rgb_alpha))) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        std::cerr << "Could not load image: " << path << std::endl;
    }
    stbi_image_free(data);

}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

void Texture::bind(GLenum target, GLenum slot) {
    glActiveTexture(slot);
    glBindTexture(target, id);
}


