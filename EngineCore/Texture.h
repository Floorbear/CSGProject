#pragma once

#include <glm/glm.hpp>
#include <list>
#include <string>
#include <glad/glad.h>

using namespace glm;

class Core;
class Texture
{
	friend Core;
public:
	Texture();
	~Texture();

public:
	//(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
	static Texture* create_texture(const ivec2& _size, const void* _data, GLint _internalformat = GL_RGB, GLenum _format = GL_RGB, GLenum _type = GL_UNSIGNED_BYTE, GLint _filtering = GL_NEAREST);
	static Texture* create_frameTexture(const ivec2& _size, GLint _internalformat = GL_RGBA, GLenum _format = GL_RGBA, GLenum _type = GL_UNSIGNED_BYTE, GLint _filtering = GL_LINEAR);
	static Texture* create_depthTexture(const ivec2& _size);

	void enable();
private:
	unsigned int texture = 0;
	
	ivec2 textureSize = {0,0};

	static std::list<Texture*> all_textures;
	//static std::map<std::string, Texture*> all_textures;

};

