#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 fragPos;


void main()
{
	//multiply : Right to left
	gl_Position = projection * view * world * vec4(aPos.x,aPos.y,aPos.z,1.0);
	fragPos = vec3(world * vec4(aPos.x,aPos.y,aPos.z,1.0 ));
	normal = mat3(transpose(inverse(world))) * aNormal;
}