#pragma once
#include "Utils.h"

enum class FrameBufferType
{
	
};


class Core;
class FrameBuffer
{
public:
	FrameBuffer();
	~FrameBuffer();

	//static create_frameBuffer();
	//virtual void enable

private:
	unsigned int fbo = 0;
	unsigned int frameBufferTexture = 0;

	static std::list<FrameBuffer*> all_frameBuffer; 
};

//class SelectionFrameBuffer

//
//SelectionFrameBuffer::enable
//{
//	FrameBuffer::enable();
//	~내꺼 호출~
//}