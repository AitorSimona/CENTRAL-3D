#ifndef __RESOURCE_SHADER_H__
#define __RESOURCE_SHADER_H__

#include "Resource.h"
#include "Math.h"

union data
{
	data()
	{
		intU = 0;
		floatU = 0;
		vec2U = { 0,0 };
		vec3U = { 0,0,0 };
		vec4U = { 0,0,0,0 };
	}

	int intU;
	float floatU;
	float2 vec2U;
	float3 vec3U;
	float4 vec4U;
};

struct Uniform
{
	Uniform() {};

	std::string name;
	uint location = 0;
	uint type; // GLenum

	data value;
};

class ResourceShader : public Resource
{
public:
	ResourceShader();
	ResourceShader(uint UID);

	// constructor reads and builds the shader
	ResourceShader(const char* vertexPath, const char* fragmentPath, bool is_extern = true);
	ResourceShader(const char* binary, uint size, uint format, const char* name, const char* vertexPath, const char* fragmentPath);
	~ResourceShader();

	void Save();

	void LoadInMemory();
	void FreeMemory();
	void ReloadAndCompileShader();
	void GetAllUniforms(std::vector<Uniform*>& uniforms);

public:
	// use/activate the shader
	void use();
	// utility uniform functions
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;

public:
	// the program ID
	unsigned int ID = 0;
	std::string name;

	std::string vShaderCode;
	std::string fShaderCode;
private:
	unsigned int vertex, fragment = 0;

	bool CreateVertexShader(unsigned int& vertex, const char * vShaderCode);
	bool CreateFragmentShader(unsigned int& fragment, const char * fShaderCode);
	bool CreateShaderProgram(unsigned int vertex, unsigned int fragment);
	bool CreateShaderProgram();
	void SaveShader();
	void DeleteShaderProgram();

	void FillUniform(Uniform* uniform, const char* name, const uint type) const;
};
#endif //__RESOURCE_SHADER_H__
