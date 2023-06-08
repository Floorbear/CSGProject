#include "PostProcessor.h"
#include "Shader.h"
#include "FrameBuffer.h"
#include "Texture.h"

GLuint PostProcessor::vao = -1;
GLuint PostProcessor::vbo_rect_vertices = -1;

vec2 PostProcessor::rect_vertices[] = {
    vec2(-1, -1),
    vec2(1, -1),
    vec2(-1,  1),
    vec2(1,  1),
};

PostProcessor::PostProcessor(std::string fragment_shader){
    if (vbo_rect_vertices == -1){ // static 멤버 초기화
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo_rect_vertices);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_rect_vertices);

        glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);

        // glBindBuffer(GL_ARRAY_BUFFER, 0);
        // glBindVertexArray(0);
    }

    shader = new Shader("postprocessing_vs.glsl", fragment_shader);
}

void PostProcessor::apply_uniforms(FrameBuffer* input){
    // empty
}

void PostProcessor::apply(FrameBuffer* input, FrameBuffer* output){
    assert(input != nullptr && output != nullptr);
    output->bind();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader->use();

    glActiveTexture(GL_TEXTURE0); // TODO : 텍스쳐쪽 구조 수정되면 정리
    input->get_texture()->enable();

    shader->set_int("fbo_input", 0);
    apply_uniforms(input);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    output->disable();
}

void OverlayPostProceesor::apply_uniforms(FrameBuffer* input){
    shader->set_float("alpha", alpha);
}

OverlayPostProceesor::OverlayPostProceesor(float alpha_) : PostProcessor("postprocessing_overlay_fs.glsl"), alpha(alpha_){
}

void Expand3PostProcessor::apply_uniforms(FrameBuffer* input){
    shader->set_vec2("unit_distance", vec2(1.0f / input->get_texture()->get_size().x, 1.0f / input->get_texture()->get_size().y));
    shader->set_vec3("color", color);
}

Expand3PostProcessor::Expand3PostProcessor(vec3 color_) : PostProcessor("postprocessing_expand3_fs.glsl"), color(color_){
}