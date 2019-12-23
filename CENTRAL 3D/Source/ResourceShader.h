#ifndef __RESOURCE_SHADER_H__
#define __RESOURCE_SHADER_H__

#include "Resource.h"

class ResourceShader : public Resource
{
public:
	ResourceShader();
	// constructor reads and builds the shader
	ResourceShader(const char* vertexPath, const char* fragmentPath, bool is_extern = true);
	ResourceShader(const char* binary, uint size);
	~ResourceShader();

	void Save();

	void LoadInMemory();
	void FreeMemory();
	void ReloadAndCompileShader();

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
};
#endif //__RESOURCE_SHADER_H__
