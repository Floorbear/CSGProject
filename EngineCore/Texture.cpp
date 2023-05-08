#include "Texture.h"

#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include <etc/stb_image.h>

Texture::Texture()
{

}
Texture::~Texture()
{
}

std::list<Texture*> Texture::all_textures;


Texture* Texture::create_texture(EnginePath& _path)
{
    ivec3 textureSize = { 0,0,0 };
    unsigned char* data = load_resouce(_path, &textureSize);
    //TODO : data 확장자 파싱해서 png, jpg 분기 나누기

    Texture* newTexture = Texture::create_texture({ textureSize.x,textureSize.y }, data, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, GL_REPEAT);
    stbi_image_free(data);//릴리즈
    return newTexture;
}

Texture* Texture::create_texture(const ivec2& _size, const void* _data, GLint _internalformat, GLenum _format, GLenum _type, GLint _filtering, GLint _wrap)
{
    Texture* newTexture = new Texture();
    glGenTextures(1, &newTexture->texture);
    glBindTexture(GL_TEXTURE_2D, newTexture->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrap);

    glTexImage2D(GL_TEXTURE_2D, 0, _internalformat, _size.x, _size.y, 0, _format, _type, _data);
    if (_data != nullptr)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    all_textures.push_back(newTexture);
    return newTexture;
}

Texture* Texture::create_frameTexture(const ivec2& _size, GLint _internalformat, GLenum _format, GLenum _type, GLint _filtering)
{
    Texture* newTexture = new Texture();
    glGenTextures(1, &(newTexture->texture));
    glBindTexture(GL_TEXTURE_2D, newTexture->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D, 0, _internalformat, _size.x, _size.y, 0, _format, _type, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newTexture->texture, 0);
   // glBindTexture(GL_TEXTURE_2D, 0);
    all_textures.push_back(newTexture);
    return newTexture;

//     glGenTextures(1, &frame_texture);
// glBindTexture(GL_TEXTURE_2D, frame_texture);
//
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//
// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
//
//// glBindTexture(GL_TEXTURE_2D, 0);
//
// glBindTexture(GL_TEXTURE_2D, frame_texture);
// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_texture, 0);
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

void Texture::enable()
{
    glBindTexture(GL_TEXTURE_2D,texture);
}

unsigned char* Texture::load_resouce(EnginePath& _path, ivec3* _return_size)
{
    //ivec3 textureSize = { 0,0,0 };
    std::string str = _path.get_path();
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(str.c_str(), &(_return_size->x), &(_return_size->y), &(_return_size->z), 0);
    return data;
}
