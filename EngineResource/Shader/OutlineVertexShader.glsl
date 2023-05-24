#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 fragPos;
out vec2 texCoord;


void main()
{
	//multiply : Right to left
	fragPos = vec3(world * vec4(aPos.x,aPos.y,aPos.z,1.0 ));
	normal = mat3(transpose(inverse(world))) * aNormal;
	gl_Position = projection * view * world * vec4(aPos + aNormal * 0.01f,1.0);
	texCoord = aTexCoord;
}