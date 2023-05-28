#version 330 core
in vec3 normal;
in vec3 fragPos;

uniform uint modelID;
uniform uint meshID;
uniform uint objectType; // 0 : model&mesh 1 : Gizmo
out uvec3 FragColor;



void main()
{             
    FragColor = uvec3(modelID, meshID, objectType);
}