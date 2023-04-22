#version 330 core
in vec3 normal;
in vec3 fragPos;
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform float ambient;

float near = 0.1; 
float far  = 10;//100.0; 
// https://heinleinsgame.tistory.com/24
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // 다시 NDC로 변환 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


void main()
{             
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // 보여주기 위해 far로 나눕니다.
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diffuse = max(dot(norm,lightDir),0);
    vec3 result = (diffuse + ambient) * objectColor;
    FragColor = vec4(result*vec3(depth), 1.0);
}