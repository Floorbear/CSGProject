#version 330 core
in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
out vec4 FragColor;

uniform int fragmentShaderType;
uniform vec3 objectColor;
uniform vec3 lightPos;
uniform float ambient;

uniform sampler2D texture1;

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

    vec3 result;
    if (fragmentShaderType == 0)
    {
        result = (diffuse + ambient) * objectColor;
    }
    else if(fragmentShaderType == 1)
    {
        result = (diffuse + ambient) * vec3(texture(texture1, texCoord));
    }
    else
    {
        result = vec3(1, 0, 1);
    }
   FragColor = vec4(result*vec3(depth), 1.0);
}