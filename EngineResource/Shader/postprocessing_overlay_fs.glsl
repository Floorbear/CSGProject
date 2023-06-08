#version 330 core

uniform sampler2D fbo_input;
uniform float alpha;

varying vec2 f_texcoord;

void main(){
  vec4 tex_color = texture2D(fbo_input, f_texcoord);
  gl_FragColor = vec4(tex_color.r, tex_color.g, tex_color.b, tex_color.a * alpha);
}
