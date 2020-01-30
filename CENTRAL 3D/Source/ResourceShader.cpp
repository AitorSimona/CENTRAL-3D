#include <fstream>
#include <sstream>
#include <iomanip>
#include "ResourceShader.h"

#include "Application.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "OpenGL.h"

#include "mmgr/mmgr.h"

ResourceShader::ResourceShader(uint UID, std::string source_file) : Resource(Resource::ResourceType::SHADER, UID, source_file)
{
	extension = ".shader";
	resource_file = SHADERS_FOLDER + std::to_string(UID) + extension;

	CreateShaderProgram();

	previewTexUID = App->gui->defaultfileTexUID;

}

ResourceShader::ResourceShader(const char * vertexPath, const char * fragmentPath, bool is_extern) : Resource(Resource::ResourceType::SHADER)
{
	bool ret = true;

	// --- Load shaders ---
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	// --- check if the shader has to be loaded from outside the engine ---
	if (is_extern)
	{
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

	}

	if (ret)
	{
		vShaderCode = vertexCode.c_str();
		fShaderCode = fragmentCode.c_str();

		if (!is_extern)
		{
			vShaderCode = vertexPath;
			fShaderCode = fragmentPath;
		}

		// 2. compile shaders
		int success = 0;
		char infoLog[512];

		success = CreateVertexShader(vertex, vShaderCode.data());

		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			CONSOLE_LOG("|[error]:Vertex Shader compilation error: %s", infoLog);
		}

		success = CreateFragmentShader(fragment, fShaderCode.data());

		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			CONSOLE_LOG("|[error]:Fragment Shader compilation error: %s", infoLog);
		}

		success = CreateShaderProgram(vertex, fragment);

		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			CONSOLE_LOG("|[error]:SHADER::PROGRAM::LINKING_FAILED: %s", infoLog);
		}

		// delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
}

ResourceShader::ResourceShader(const char * binary, uint size, uint format, const char* name, const char* vertexPath, const char* fragmentPath) : Resource(Resource::ResourceType::SHADER)
{
	int success;
	char infoLog[512];

	// --- Load shader's code ---
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
		vShaderCode = vShaderStream.str();
		fShaderCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		CONSOLE_LOG("|[error]:SHADER:FILE_NOT_SUCCESFULLY_READ: %s", e.what());
	}

	success = CreateShaderProgram();
	this->name = name;

	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		CONSOLE_LOG("|[error]:SHADER::PROGRAM::LINKING_FAILED: %s", infoLog);

		glDeleteProgram(ID);
	}
	else
	{
		glProgramBinary(ID, (GLenum)format, (void*)binary, (GLint)size);

		// print linking errors if any
		glGetProgramiv(ID, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			CONSOLE_LOG("|[error]: Could not load binary shader, triggered recompilation %s", infoLog);

			// --- Trigger recompilation ---
			glDeleteProgram(ID);
			CreateShaderProgram();
			ReloadAndCompileShader();
		}
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

void ResourceShader::ReloadAndCompileShader()
{
	uint new_vertex, new_fragment = 0;

	// --- Compile new data ---

	const char* vertexcode = vShaderCode.c_str();
	const char* fragmentcode = fShaderCode.c_str();

	GLint success = 0;
	GLint accumulated_errors = 0;
	char infoLog[512];

	// --- Compile new vertex shader ---

	success = CreateVertexShader(new_vertex, vertexcode);

	if (!success)
	{
		glGetShaderInfoLog(new_vertex, 512, NULL, infoLog);
		CONSOLE_LOG("|[error]:Vertex Shader compilation error: %s", infoLog);
		accumulated_errors++;
	}
	else
		CONSOLE_LOG("Vertex Shader compiled successfully");

	// --- Compile new fragment shader ---

	success = CreateFragmentShader(new_fragment,fragmentcode);

	if (!success)
	{
		glGetShaderInfoLog(new_fragment, 512, NULL, infoLog);
		CONSOLE_LOG("|[error]:Fragment Shader compilation error: %s", infoLog);
		accumulated_errors++;
	}
	else
		CONSOLE_LOG("Fragment Shader compiled successfully");

	if (accumulated_errors == 0)
	{
		// --- Delete previous shader data ---
		glDetachShader(ID, vertex);
		glDetachShader(ID, fragment);

		// --- Attach new shader objects and link ---
		glAttachShader(ID, new_vertex);
		glAttachShader(ID, new_fragment);
		glLinkProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			CONSOLE_LOG("|[error]:SHADER::PROGRAM::LINKING_FAILED: %s", infoLog);

			// --- Detach new shader objects ---
			glDetachShader(ID, new_vertex);
			glDetachShader(ID, new_fragment);
			glDeleteShader(new_vertex);
			glDeleteShader(new_fragment);

			// --- Attach old shader objects ---
			glAttachShader(ID, vertex);
			glAttachShader(ID, fragment);
		}
		else
		{
			// --- On success, delete old shader objects and update ids ---
			glDeleteShader(vertex);
			glDeleteShader(fragment);
			vertex = new_vertex;
			fragment = new_fragment;

			CONSOLE_LOG("Shader Program linked successfully");
		}
	}
	else
	{
		glDeleteShader(new_vertex);
		glDeleteShader(new_fragment);
	}


}

void ResourceShader::GetAllUniforms(std::vector<Uniform*>& uniforms)
{
	std::vector<Uniform*> new_uniforms;

	int uniform_count;
	glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &uniform_count);

	char name[128];
	GLenum type;
	GLint size;

	for (uint i = 0; i < uniform_count; ++i)
	{
		glGetActiveUniform(ID, i, 128, nullptr, &size, &type, name);

		if (strcmp(name, "model_matrix") == 0
			|| strcmp(name, "view") == 0
			|| strcmp(name, "projection") == 0
			|| strcmp(name, "time") == 0)
			continue;

		Uniform* uniform = nullptr;

		// --- This switch prevents retrieving unwanted/unsupported uniforms ---
		// MYTODO: we may avoid the switch by defining some filters and testing against them

		switch (type)
		{
		case GL_INT:
			uniform = new Uniform();
			FillUniform(uniform,name,type);
			break;

		case GL_FLOAT:
			uniform = new Uniform();
			FillUniform(uniform, name, type);
			break;
		
		case GL_FLOAT_VEC2:
			uniform = new Uniform();
			FillUniform(uniform, name, type);
			break;

		case GL_FLOAT_VEC3:
			uniform = new Uniform();
			FillUniform(uniform, name, type);
			break;

		case GL_FLOAT_VEC4:
			uniform = new Uniform();
			FillUniform(uniform, name, type);
			break;

		case GL_INT_VEC2:
			uniform = new Uniform();
			FillUniform(uniform, name, type);
			break;

		case GL_INT_VEC3:
			uniform = new Uniform();
			FillUniform(uniform, name, type);
			break;

		case GL_INT_VEC4:
			uniform = new Uniform();
			FillUniform(uniform, name, type);
			break;

		default:
			continue;
			break;

		}

		// --- Conserve previous uniform values if they still exist ---
		for (uint i = 0; i < uniforms.size(); ++i)
		{
			if (uniforms[i]->name == uniform->name && uniforms[i]->type == uniform->type)
				uniform->value = uniforms[i]->value;
		}

		new_uniforms.push_back(uniform);
	}

	// --- Delete previous uniforms and retrieve new ones ---
	for (uint i = 0; i < uniforms.size(); ++i)
	{
		delete uniforms[i];
	}

	uniforms.clear();

	uniforms = new_uniforms;
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
	GLint success = 1;

	// shader Program
	ID = glCreateProgram();
	//glLinkProgram(ID);
	//// print linking errors if any
	//glGetProgramiv(ID, GL_LINK_STATUS, &success);

	return success;
}

// Internal use only!
void ResourceShader::SaveShader()
{
//	std::string path = SHADERS_FOLDER;
//	path.append(std::to_string(UID));
//	path.append(".shader");
//
//	GLint buffer_size;
//	glGetProgramiv(ID,GL_PROGRAM_BINARY_LENGTH, &buffer_size);
//
//	if (buffer_size > 0)
//	{
//		char* buffer = new char[buffer_size];
//		GLint bytes_written = 0;
//		GLenum format = 0;
//
//		glGetProgramBinary(ID, buffer_size, &bytes_written, &format, buffer);
//
//		if (bytes_written > 0)
//		{
//			// --- Save shader to file ---
//			App->fs->Save(path.data(), buffer, buffer_size);
//
//			std::string binary_format;
//			path.append(".format");
//			json jsonfile;
//			char* bin_buffer = nullptr;
//
//			// --- Create Meta ---
//			jsonfile["FORMAT"] = std::to_string(format);
//			jsonfile["NAME"] = name;
//			binary_format = App->GetJLoader()->Serialize(jsonfile);
//			bin_buffer = (char*)binary_format.data();
//
//			App->fs->Save(path.data(), bin_buffer, binary_format.size());
//;
//
//			// --- Save code to Assets folder, save meta ---
//			path = ASSETS_FOLDER;
//			path.append("Shaders/");
//			path.append(name);
//
//			std::string final_name = path + ".vertex";
//
//			if(!App->resources->IsFileImported(final_name.data()))
//			App->resources->CreateMetaFromUID(UID, final_name.data());
//
//			std::ofstream file;
//			file.open(final_name);
//
//			if (!file.is_open())
//			{
//				CONSOLE_LOG("|[error]: JSONLoader::Save could not open File: %s", final_name.data());
//			}
//			else
//			{
//				file << std::setw(4) << vShaderCode << std::endl;
//				file.close();
//			}
//
//			final_name = path + ".fragment";
//
//			if (!App->resources->IsFileImported(final_name.data()))
//			App->resources->CreateMetaFromUID(UID, final_name.data());
//
//			std::ofstream file2;
//			file2.open(final_name);
//
//			if (!file2.is_open())
//			{
//				CONSOLE_LOG("|[error]: JSONLoader::Save could not open File: %s", final_name.data());
//			}
//			else
//			{
//				file2 << std::setw(4) << fShaderCode << std::endl;
//				file2.close();
//			}
//		}
//
//		delete[] buffer;
//	}
//	else
//		CONSOLE_LOG("|[error]: Could not save Shader: %s, try compiling it first", name.data());
}

void ResourceShader::DeleteShaderProgram()
{
	if (glIsProgram(ID))
	{
		glDeleteProgram(ID);
		ID = 0;
	}
}

void ResourceShader::FillUniform(Uniform* uniform, const char* name, const uint type) const
{
	 uniform->name = name;
	 uniform->location = glGetUniformLocation(ID, name);
	 uniform->type = type;
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
