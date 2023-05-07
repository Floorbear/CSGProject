#include "FrameBuffer.h"
#include <glad/glad.h>
#include "Texture.h"
FrameBuffer::FrameBuffer()
{
}

FrameBuffer::~FrameBuffer()
{
}

FrameBuffer* FrameBuffer::create_frameBuffer()
{
	FrameBuffer* newFrameBuffer = new FrameBuffer();
	glGenFramebuffers(1, &newFrameBuffer->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, newFrameBuffer->fbo);
	all_frameBuffer.push_back(newFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return newFrameBuffer;
}

void FrameBuffer::enable()
{
	assert(fbo != 0);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void FrameBuffer::disable()
{
	assert(fbo != 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::list<FrameBuffer*> FrameBuffer::all_frameBuffer;

SelectionFrameBuffer::SelectionFrameBuffer()
{
}

SelectionFrameBuffer::~SelectionFrameBuffer()
{
}

SelectionFrameBuffer* SelectionFrameBuffer::create_selectionFrameBuffer(const ivec2& _texture_size)
{
	SelectionFrameBuffer* newFramebuffer = static_cast<SelectionFrameBuffer*>(create_frameBuffer());
	newFramebuffer->enable();
	newFramebuffer->framebufferTexture = Texture::create_frameTexture(ivec2((int)_texture_size.x, (int)_texture_size.y), GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT);
	Texture* newdepthTexture = Texture::create_depthTexture(ivec2((int)_texture_size.x, (int)_texture_size.y));
	auto Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	assert(Status == GL_FRAMEBUFFER_COMPLETE); //버퍼가 정상적으로 생성됬는지 체크

	glBindTexture(GL_TEXTURE_2D, 0);
	newFramebuffer->disable();
	return newFramebuffer;
}

SelectionPixelIdInfo SelectionFrameBuffer::read_pixel(int _x, int _y)
{
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	SelectionPixelIdInfo Pixel;
	glReadPixels(_x, _y,
		1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &Pixel);

	glReadBuffer(GL_NONE);
	return Pixel;
}

ScreenFrameBuffer::ScreenFrameBuffer()
{
}

ScreenFrameBuffer::~ScreenFrameBuffer()
{
}

ScreenFrameBuffer* ScreenFrameBuffer::create_screenFrameBuffer(const ivec2& _texture_size)
{
	ScreenFrameBuffer* newFramebuffer = static_cast<ScreenFrameBuffer*>(create_frameBuffer());
	newFramebuffer->enable();
	newFramebuffer->framebufferTexture = Texture::create_frameTexture(_texture_size, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR);
	newFramebuffer->framebufferTexture->enable();

	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _texture_size.x, _texture_size.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	return newFramebuffer;
}
