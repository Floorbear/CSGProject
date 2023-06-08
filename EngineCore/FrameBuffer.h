#pragma once
#include "SelectionPixelInfo.h"

#include "glm/glm.hpp"

#include <list>

using namespace glm;

class Core;
class Texture;
class FrameBuffer{
    static std::list<FrameBuffer*> instances;
    static void dispose();

protected:
    unsigned int fbo = 0;
    Texture* texture = nullptr;

    FrameBuffer();

public:
    unsigned int get_fbo();
    Texture* get_texture();

    void bind();
    void disable();

    friend Core;
};

class ScreenFrameBuffer : public FrameBuffer{
public:
    ScreenFrameBuffer(const ivec2& texture_size);
    ~ScreenFrameBuffer(); // TODO : 버퍼 지우기...
};

class SelectionFrameBuffer : public FrameBuffer{
    Texture* texture_depth = nullptr;

public:
    SelectionFrameBuffer(const ivec2& texture_size);
    ~SelectionFrameBuffer();

    SelectionPixelIdInfo read_pixel(float x, float y);
};
