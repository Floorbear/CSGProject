#include "FrameBuffer.h"
#include "Texture.h"

#include <glad/glad.h>

std::list<FrameBuffer*> FrameBuffer::instances;

void FrameBuffer::dispose(){
    for (FrameBuffer* framebuffer : instances){
        delete framebuffer;
    }
}

FrameBuffer::FrameBuffer(){
    instances.push_back(this);

    glGenFramebuffers(1, &fbo);
}

unsigned int FrameBuffer::get_fbo(){
    return fbo;
}

Texture* FrameBuffer::get_texture(){
    assert(texture != nullptr);
    return texture;
}

void FrameBuffer::bind(){
    assert(fbo != 0);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void FrameBuffer::disable(){
    assert(fbo != 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ScreenFrameBuffer::ScreenFrameBuffer(const ivec2& texture_size){
    bind();
    texture = Texture::create_frameTexture(texture_size, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR);
    texture->enable();

    GLuint depth_stencil_renderbuffer;
    glGenRenderbuffers(1, &depth_stencil_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, texture_size.x, texture_size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_renderbuffer);

    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(status == GL_FRAMEBUFFER_COMPLETE);
}

SelectionFrameBuffer::SelectionFrameBuffer(const ivec2& texture_size_){
    bind();
    texture = Texture::create_frameTexture(texture_size_, GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT);
    texture_depth = Texture::create_depthTexture(texture_size_); // ???

    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(status == GL_FRAMEBUFFER_COMPLETE);
}

SelectionPixelIdInfo SelectionFrameBuffer::read_pixel(float x, float y){
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    SelectionPixelIdInfo pixel;
    glReadPixels((int)x, (int)y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &pixel);
    glReadBuffer(GL_NONE);

    return pixel;
}