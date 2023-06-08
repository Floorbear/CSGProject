#version 330 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_texcoord;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

void main(){
	gl_Position = projection * view * world * vec4(v_pos, 1.0);
}
