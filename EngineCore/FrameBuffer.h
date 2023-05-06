#pragma once
#include "Utils.h"
#include "SelectionPixelInfo.h"
enum class FrameBufferType
{
	
};


class Core;
class FrameBuffer
{
	friend Core;
public:
	FrameBuffer();
	virtual ~FrameBuffer();

	static FrameBuffer* create_frameBuffer();
	virtual void enable();
	virtual void disable();

protected:
	unsigned int fbo = 0;
	unsigned int frameBufferTexture = 0;

private:
	static std::list<FrameBuffer*> all_frameBuffer; 
};

class ScreenFrameBuffer : public FrameBuffer
{
public:
	ScreenFrameBuffer();
	virtual ~ScreenFrameBuffer();

	static ScreenFrameBuffer* create_screenFrameBuffer(const ivec2& _texture_size);
};

class SelectionFrameBuffer : public FrameBuffer
{
public:
	SelectionFrameBuffer();
	virtual ~SelectionFrameBuffer();

	static SelectionFrameBuffer* create_selectionFrameBuffer(const ivec2& _texture_size);

	SelectionPixelIdInfo read_pixel(int _x, int _y);


};
