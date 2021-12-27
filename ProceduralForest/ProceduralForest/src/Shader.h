#pragma once
#include <string>
#include <unordered_map>
#include <glm.hpp>
#include "vendor/glugg/VectorUtils3.h"

class Shader
{
private:
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;


public:
	Shader(const std::string& filepathVert, const std::string& filepathFrag);
	~Shader();

	void Bind() const; 
	void UnBind() const;

	//Set uniforms
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
	void SetUniformMat4fVU(const std::string& name, const mat4& matrix);

	unsigned int GetRendererID() {
		return m_RendererID;
	}

private:
	std::string ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	int GetUniformLocation(const std::string& name);
};

