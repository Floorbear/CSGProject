#pragma once

#include <string>
#include <map>

#include <glm/glm.hpp>

using namespace glm;

class Shader{
	unsigned int vertexShader = 0;
	unsigned int fragmentShader = 0;
	unsigned int shaderProgram = 0;

	static std::map<std::string, std::string> compileData;

public:
	Shader(std::string vertexShader_ = "DefaultVertexShader.glsl", std::string fragmentShader_ ="DefaultFragmentShader.glsl"); //확장자 포함해서 적어주세요
	~Shader();

	void use();

	inline unsigned int get_program()
	{
		return shaderProgram;
	}

	void set_int(std::string_view uniform_, int _int);
	void set_uint(std::string_view uniform_, unsigned int _uint);
	void set_float(std::string_view uniform_, float value_);
	void set_vec2(std::string_view uniform_, const glm::vec2& value_);
	void set_vec3(std::string_view uniform_, const glm::vec3& value_);
	void set_mat4(std::string_view uniform_, const glm::mat4& value_);
};
