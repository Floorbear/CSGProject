#include "Texture.h"

#include "glad/glad.h"

Texture::Texture()
{

}
Texture::~Texture()
{
}

std::list<Texture*> Texture::all_textures;


Texture* Texture::create_texture(const ivec2& _size, const void* _data, GLint _internalformat, GLenum _format, GLenum _type, GLint _filtering)
{
    Texture* newTexture = new Texture();
    glGenTextures(1, &newTexture->texture);
    glBindTexture(GL_TEXTURE_2D, newTexture->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D, 0, _internalformat, _size.x, _size.y, 0, _format, _type, _data);

    glBindTexture(GL_TEXTURE_2D, 0);
    all_textures.push_back(newTexture);
    return newTexture;
}

Texture* Texture::create_frameTexture(const ivec2& _size, GLint _internalformat, GLenum _format, GLenum _type, GLint _filtering)
{
    Texture* newTexture = new Texture();
    glGenTextures(1, &newTexture->texture);
    glBindTexture(GL_TEXTURE_2D, newTexture->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D, 0, _internalformat, _size.x, _size.y, 0, _format, _type, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newTexture->texture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    all_textures.push_back(newTexture);
    return newTexture;
}

Texture* Texture::create_depthTexture(const ivec2& _size)
{
    Texture* newTexture = new Texture();
    glGenTextures(1, &newTexture->texture);
    glBindTexture(GL_TEXTURE_2D, newTexture->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _size.x, _size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, newTexture->texture, 0);
    all_textures.push_back(newTexture);
    glBindTexture(GL_TEXTURE_2D, 0);
    return newTexture;
}
