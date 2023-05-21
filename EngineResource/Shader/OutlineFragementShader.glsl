#version 330 core
in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
out vec4 FragColor;



void main()
{  
	vec3 outlineColor = vec3(1.0, 1.0, 1);
   FragColor = vec4(outlineColor, 1.0);
}