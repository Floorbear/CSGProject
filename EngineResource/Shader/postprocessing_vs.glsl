#version 330 core

layout (location = 0) in vec2 v_pos;

varying vec2 f_texcoord;

void main(){
  gl_Position = vec4(v_pos, 0.0, 1.0);
  f_texcoord = (v_pos + 1.0) / 2.0;
}
