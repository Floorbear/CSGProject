#version 330 core
in vec3 normal;
in vec3 fragPos;

uniform uint objectID;
out uvec3 FragColor;



void main()
{             
    FragColor = uvec3(objectID, 0, 0);
}