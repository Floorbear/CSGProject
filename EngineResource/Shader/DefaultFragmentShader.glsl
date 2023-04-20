#version 330 core
out vec4 FragColor;

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
    FragColor = vec4(vec3(depth), 1.0);
}