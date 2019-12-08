#include <fstream>
#include <sstream>
#include "ResourceShader.h"

#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "OpenGL.h"

#include "mmgr/mmgr.h"

ResourceShader::ResourceShader() : Resource(Resource::ResourceType::SHADER)
{

}

ResourceShader::ResourceShader(const char * vertexPath, const char * fragmentPath) : Resource(Resource::ResourceType::SHADER)
{
	// MYTODO: Clean this and use physFS
	bool ret = true;

	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		CONSOLE_LOG("|[error]:SHADER:FILE_NOT_SUCCESFULLY_READ: %s", e.what());
		ret = false;
	}

	if (ret)
	{

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// 2. compile shaders
		unsigned int vertex, fragment = 0;
		int success = 0;
		char infoLog[512];

		success = CreateVertexShader(vertex, vShaderCode);

		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			CONSOLE_LOG("|[error]:Vertex Shader compilation error: %s", infoLog);
		};

		success = CreateFragmentShader(fragment, fShaderCode);

		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			CONSOLE_LOG("|[error]:Fragment Shader compilation error: %s", infoLog);
		};

		success = CreateShaderProgram(vertex, fragment);

		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			CONSOLE_LOG("|[error]:SHADER::PROGRAM::LINKING_FAILED: %s", infoLog);
		}

		// --- Save Shaders to lib --- 
		SaveShader();

		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
}

ResourceShader::ResourceShader(const char * binary, uint size) : Resource(Resource::ResourceType::SHADER)
{
	int success;
	char infoLog[512];

	success = CreateShaderProgram();

	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		CONSOLE_LOG("|[error]:SHADER::PROGRAM::LINKING_FAILED: %s", infoLog);
	}
	else
	{
		glProgramBinary(ID, 0, binary, size);
	}
}

ResourceShader::~ResourceShader()
{
	DeleteShaderProgram();
}

void ResourceShader::Save()
{
	SaveShader();
}

void ResourceShader::LoadInMemory()
{
}

void ResourceShader::FreeMemory()
{
}

// Internal use only!
bool ResourceShader::CreateVertexShader(unsigned int& vertex, const char * vShaderCode)
{
	GLint success = 0;

	// vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

	return success;
}

// Internal use only!
bool ResourceShader::CreateFragmentShader(unsigned int& fragment, const char * fShaderCode)
{
	GLint success = 0;

	// similiar for Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

	return success;
}

// Internal use only!
bool ResourceShader::CreateShaderProgram(unsigned int vertex, unsigned int fragment)
{
	GLint success = 0;

	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	// print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);

	return success;
}

bool ResourceShader::CreateShaderProgram()
{
	GLint success = 0;

	// shader Program
	ID = glCreateProgram();
	glLinkProgram(ID);
	// print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);

	return success;
}

// Internal use only!
void ResourceShader::SaveShader()
{
	std::string path = SHADERS_FOLDER;
	path.append(std::to_string(UID));

	//if(App->fs->Exists(path.data()))
	GLint buffer_size;
	glGetProgramiv(ID,GL_PROGRAM_BINARY_LENGTH, &buffer_size);

	char* buffer = new char[buffer_size];
	GLint bytes_written = 0;
	GLenum format = 0;

	glGetProgramBinary(ID, buffer_size, &bytes_written, &format, buffer);

	if (bytes_written > 0)
	{
		// --- Save shader to file ---
		App->fs->Save(path.data(), buffer, buffer_size);
	}

	delete[] buffer;
}

void ResourceShader::DeleteShaderProgram()
{
	if (glIsProgram(ID))
	{
		glDeleteProgram(ID);
		ID = 0;
	}
}

void ResourceShader::use()
{
	glUseProgram(ID);
}

void ResourceShader::setBool(const std::string & name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void ResourceShader::setInt(const std::string & name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void ResourceShader::setFloat(const std::string & name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
