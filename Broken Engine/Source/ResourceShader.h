#ifndef __RESOURCE_SHADER_H__
#define __RESOURCE_SHADER_H__

#include "Resource.h"
#include "Math.h"

BE_BEGIN_NAMESPACE
union BROKEN_API data
{
	data() {
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

struct BROKEN_API Uniform 
{
	Uniform() {};

	std::string name;
	uint location = 0;
	uint type = 0; // GLenum

	data value;
};

enum class UniformType
{
	intU,
	floatU,
	vec2U,
	vec3U,
	vec4U,
	vec4x4U
};

class BROKEN_API ResourceShader : public Resource 
{
public:
	ResourceShader(uint UID, const char* source_file);
	~ResourceShader();

	bool LoadInMemory() override;
	void FreeMemory() override;

	// --- Getters ---
	void GetAllUniforms(std::vector<Uniform*>& uniforms);

	// --- Setters ---
	void setBool(const std::string& name, bool value) const;
	void setUniform(const char* name, data& unidata, UniformType UniType) const;

	// --- Utilities ---
	void use();
	void ReloadAndCompileShader();
	void FillUniform(Uniform* uniform, const char* name, const uint type) const;

public:
	// the program ID
	unsigned int ID = 0;
	std::string ShaderCode;
	std::string vShaderCode;
	std::string fShaderCode;
private:
	unsigned int vertex, fragment = 0;

	bool CreateVertexShader(unsigned int& vertex, const char* vShaderCode);
	bool CreateFragmentShader(unsigned int& fragment, const char* fShaderCode);
	bool CreateShaderProgram(unsigned int vertex, unsigned int fragment);
	bool CreateShaderProgram();
	void DeleteShaderProgram();


	bool LoadStream(const char* path);
private:
	void OnOverwrite() override;
	void OnDelete() override;
};
BE_END_NAMESPACE
#endif //__RESOURCE_SHADER_H__
