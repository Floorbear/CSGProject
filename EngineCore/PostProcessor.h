#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <string>

using namespace glm;

class Shader;
class FrameBuffer;
class PostProcessor{
    static GLuint vao;
    static GLuint vbo_rect_vertices;
    static vec2 rect_vertices[];

protected:
    Shader* shader;

    PostProcessor(std::string fragment_shader);

    virtual void apply_uniforms(FrameBuffer* input);

public:
    virtual void apply(FrameBuffer* input, FrameBuffer* output);
};


class OverlayPostProceesor : public PostProcessor{
    float alpha;

    void apply_uniforms(FrameBuffer* input) override;

public:
    OverlayPostProceesor(float alpha_);
};

class Expand3PostProcessor : public PostProcessor{
    vec3 color;

    void apply_uniforms(FrameBuffer* input) override;

public:
    Expand3PostProcessor(vec3 color_);
};
