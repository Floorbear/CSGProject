#version 330 core
in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
out vec4 FragColor;

uniform vec3 gizmoColor;



void main()
{  
   FragColor = vec4(gizmoColor, 1.0);
}