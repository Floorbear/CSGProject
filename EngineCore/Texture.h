#pragma once

#include <glm/glm.hpp>
#include <list>
#include <string>
#include <glad/glad.h>
#include "FileSystem.h"

using namespace glm;

class Core;
class Texture
{
	friend Core;
public:
	Texture();
	~Texture();

	inline unsigned int get_texture_handle()
	{
		assert(texture != 0);
		return texture;
	}
public:
	//(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
	static Texture* create_texture(EnginePath _path, bool flip = true);
	static Texture* create_texture(EnginePath _path, GLint _internalformat, GLenum _format = GL_RGB, bool flip = true);
	static Texture* create_texture(const ivec2& _size, const void* _data, GLint _internalformat = GL_RGB, GLenum _format = GL_RGB, GLenum _type = GL_UNSIGNED_BYTE, GLint _filtering = GL_NEAREST, GLint _wrap = GL_CLAMP_TO_BORDER);
	static Texture* create_frameTexture(const ivec2& _size, GLint _internalformat = GL_RGBA, GLenum _format = GL_RGBA, GLenum _type = GL_UNSIGNED_BYTE, GLint _filtering = GL_LINEAR);
	static Texture* create_depthTexture(const ivec2& _size);

	void enable();

	static unsigned char* load_resouce(EnginePath& _path, ivec3* _return_size, bool flip);//_return_size에 텍스처의 width, height, 채널이 리턴됨
private:
	unsigned int texture = 0;
	
	unsigned char* resourceData = nullptr;
	ivec3 textureSize = {0,0,0};//width,height, channel

	static std::list<Texture*> all_textures;
	//static std::map<std::string, Texture*> all_textures;

};

