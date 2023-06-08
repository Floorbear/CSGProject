#version 330 core

uniform sampler2D fbo_input;
uniform vec2 unit_distance;
uniform vec3 color;

varying vec2 f_texcoord;

void main(){
  vec4 tex_color = texture2D(fbo_input, f_texcoord)
  
  // 좌상
    + texture2D(fbo_input, vec2(f_texcoord.x - unit_distance.x,     f_texcoord.y))
    + texture2D(fbo_input, vec2(f_texcoord.x - unit_distance.x * 2, f_texcoord.y))
    + texture2D(fbo_input, vec2(f_texcoord.x - unit_distance.x * 3, f_texcoord.y))
    + texture2D(fbo_input, vec2(f_texcoord.x - unit_distance.x,     f_texcoord.y + unit_distance.y))
    + texture2D(fbo_input, vec2(f_texcoord.x - unit_distance.x,     f_texcoord.y + unit_distance.y * 2))
    + texture2D(fbo_input, vec2(f_texcoord.x - unit_distance.x * 2, f_texcoord.y + unit_distance.y))

  // 우상
    + texture2D(fbo_input, vec2(f_texcoord.x,                       f_texcoord.y + unit_distance.y))
    + texture2D(fbo_input, vec2(f_texcoord.x,                       f_texcoord.y + unit_distance.y * 2))
    + texture2D(fbo_input, vec2(f_texcoord.x,                       f_texcoord.y + unit_distance.y * 3))
    + texture2D(fbo_input, vec2(f_texcoord.x + unit_distance.x,     f_texcoord.y + unit_distance.y))
    + texture2D(fbo_input, vec2(f_texcoord.x + unit_distance.x,     f_texcoord.y + unit_distance.y * 2))
    + texture2D(fbo_input, vec2(f_texcoord.x + unit_distance.x * 2, f_texcoord.y + unit_distance.y))

  // 우하
    + texture2D(fbo_input, vec2(f_texcoord.x + unit_distance.x,     f_texcoord.y))
    + texture2D(fbo_input, vec2(f_texcoord.x + unit_distance.x * 2, f_texcoord.y))
    + texture2D(fbo_input, vec2(f_texcoord.x + unit_distance.x * 3, f_texcoord.y))
    + texture2D(fbo_input, vec2(f_texcoord.x + unit_distance.x,     f_texcoord.y - unit_distance.y))
    + texture2D(fbo_input, vec2(f_texcoord.x + unit_distance.x,     f_texcoord.y - unit_distance.y * 2))
    + texture2D(fbo_input, vec2(f_texcoord.x + unit_distance.x * 2, f_texcoord.y - unit_distance.y))

  // 좌하
    + texture2D(fbo_input, vec2(f_texcoord.x,                       f_texcoord.y - unit_distance.y))
    + texture2D(fbo_input, vec2(f_texcoord.x,                       f_texcoord.y - unit_distance.y * 2))
    + texture2D(fbo_input, vec2(f_texcoord.x,                       f_texcoord.y - unit_distance.y * 3))
    + texture2D(fbo_input, vec2(f_texcoord.x - unit_distance.x,     f_texcoord.y - unit_distance.y))
    + texture2D(fbo_input, vec2(f_texcoord.x - unit_distance.x,     f_texcoord.y - unit_distance.y * 2))
    + texture2D(fbo_input, vec2(f_texcoord.x - unit_distance.x * 2, f_texcoord.y - unit_distance.y));

  tex_color.a = clamp(tex_color.a, 0, 1);
  gl_FragColor = vec4(color, tex_color.a);
}
