#include <fstream>
#include <sstream>
#include <iomanip>

#include "ResourceShader.h"
#include "ImporterMeta.h"
#include "ResourceMeta.h"

#include "Application.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"
#include "ModuleRenderer3D.h"
#include "JSONLoader.h"

#include "OpenGL.h"

#include "mmgr/mmgr.h"

ResourceShader::ResourceShader(uint UID, std::string source_file) : Resource(Resource::ResourceType::SHADER, UID, source_file)
{
	extension = ".glsl";
	resource_file = SHADERS_FOLDER + std::to_string(UID) + extension;

	vShaderCode = App->renderer3D->VertexShaderTemplate;
	fShaderCode = App->renderer3D->FragmentShaderTemplate;

	CreateShaderProgram();

	previewTexID = App->gui->shaderTexID;
}

ResourceShader::~ResourceShader()
{
	//DeleteShaderProgram();
}


bool ResourceShader::LoadInMemory()
{
	bool ret = true;

	// --- Retrieve data from meta ---
	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(original_file.c_str());

	// --- Load from binary file ---
	if (meta && meta->Date == App->fs->GetLastModificationTime(this->original_file.c_str()))
	{
		int success;
		char infoLog[512];

		ret = LoadStream(resource_file.c_str());

		if (ret)
		{
			// --- Create empty program ---
			DeleteShaderProgram();

			success = CreateShaderProgram();

			if (!success)
			{
				glGetProgramInfoLog(ID, 512, NULL, infoLog);
				CONSOLE_LOG("|[error]:SHADER::PROGRAM::LINKING_FAILED: %s", infoLog);

				glDeleteProgram(ID);
			}
			else
			{
				uint format = meta->ResourceData["FORMAT"].is_null() ? 0 : meta->ResourceData["FORMAT"].get<uint>();

				char* buffer = nullptr;

				uint size = App->fs->Load(resource_file.c_str(), &buffer);

				// --- Load binary program ---
				glProgramBinary(ID, (GLenum)format, (void*)buffer, (GLint)size);

				if (buffer)
					delete[] buffer;



				// --- Print linking errors if any ---
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

				if (App->fs->Exists(original_file.c_str()))
				{
					// --- Load original code ---
					LoadStream(original_file.c_str());

					// --- Separate vertex and fragment ---
					std::string ftag = "#define FRAGMENT_SHADER";
					uint FragmentLoc = ShaderCode.find(ftag);

					vShaderCode = ShaderCode.substr(0, FragmentLoc - 1);
					fShaderCode = std::string("#version 440 core\n").append(ShaderCode.substr(FragmentLoc, ShaderCode.size()));
				}
			}
		}
	}

	// --- Load glsl file ---
	else
	{
		ret = LoadStream(original_file.c_str());

		// --- If no fs failure occurred... ---
		if (ret)
		{
			DeleteShaderProgram();

			// --- Separate vertex and fragment ---
			std::string ftag = "#define FRAGMENT_SHADER";
			uint FragmentLoc = ShaderCode.find(ftag);

			vShaderCode = ShaderCode.substr(0, FragmentLoc - 1);
			fShaderCode = std::string("#version 440 core\n").append(ShaderCode.substr(FragmentLoc, ShaderCode.size()));

			// --- Compile shaders ---
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

	return ret;
}

void ResourceShader::FreeMemory()
{
	DeleteShaderProgram();
}

void ResourceShader::CreateInspectorNode()
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

	success = CreateFragmentShader(new_fragment, fragmentcode);

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
		//glValidateProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);

		if (!success)
		{
			GLint length;
			glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &length);
			std::vector<GLchar> message(length);
			glGetProgramInfoLog(ID, length, &length, &message[0]);

			CONSOLE_LOG("Error in Shader Program Linking: %s", message.data());

			//glGetProgramInfoLog(ID, 512, NULL, infoLog);
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
			|| strcmp(name, "time") == 0
			|| strcmp(name, "Color") == 0
			|| strcmp(name, "Texture") == 0
			|| strcmp(name, "u_Shininess") == 0
			|| strcmp(name, "u_LightsNumber") == 0
			|| strcmp(name, "u_CameraPosition") == 0
			|| std::string(name).find("u_BkLights") != std::string::npos)
			continue;

		Uniform* uniform = nullptr;

		// --- This switch prevents retrieving unwanted/unsupported uniforms ---
		// MYTODO: we may avoid the switch by defining some filters and testing against them

		switch (type)
		{
		case GL_INT:
			uniform = new Uniform();
			FillUniform(uniform, name, type);
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
bool ResourceShader::CreateVertexShader(unsigned int& vertex, const char* vShaderCode)
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
bool ResourceShader::CreateFragmentShader(unsigned int& fragment, const char* fShaderCode)
{
	GLint success = 0;

	// similar for Fragment Shader
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
	char infoLog[512];

	// shader Program
	ID = glCreateProgram();
	//glLinkProgram(ID);
	//// print linking errors if any
	//glGetProgramiv(ID, GL_LINK_STATUS, &success);

	//if (!success)
	//{
	//	glGetShaderInfoLog(ID, 512, NULL, infoLog);
	//	ENGINE_CONSOLE_LOG("|[error]: Shader creation error: %s", infoLog);
	//}
	//else
	//	ENGINE_CONSOLE_LOG("Shader created successfully");

	return success;
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

bool ResourceShader::LoadStream(const char* path)
{
	bool ret = true;

	// --- Create input stream object ---
	std::ifstream ShaderFile;

	// --- Ensure ifstream objects can throw exceptions ---
	ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// --- Open files ---
		ShaderFile.open(path);
		std::stringstream ShaderStream;
		// --- Read file's buffer contents into streams ---
		ShaderStream << ShaderFile.rdbuf();
		// --- Close file handlers ---
		ShaderFile.close();
		// --- Convert stream into string ---
		ShaderCode = ShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		CONSOLE_LOG("|[error]:SHADER:FILE_NOT_SUCCESFULLY_READ: %s", e.what());
		ret = false;
	}

	return ret;
}

void ResourceShader::OnOverwrite()
{
	NotifyUsers(ResourceNotificationType::Overwrite);

	bool ret = LoadStream(original_file.c_str());

	// --- If no fs failure occurred... ---
	if (ret)
	{
		// --- Separate vertex and fragment ---
		std::string ftag = "#define FRAGMENT_SHADER";
		uint FragmentLoc = ShaderCode.find(ftag);

		vShaderCode = ShaderCode.substr(0, FragmentLoc - 1);
		fShaderCode = std::string("#version 440 core\n").append(ShaderCode.substr(FragmentLoc, ShaderCode.size()));
	}

	ReloadAndCompileShader();

	//FreeMemory();
	//LoadInMemory();
}

void ResourceShader::OnDelete()
{
	NotifyUsers(ResourceNotificationType::Deletion);

	FreeMemory();
	App->fs->Remove(resource_file.c_str());
	App->fs->Remove(original_file.c_str());

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}

void ResourceShader::use()
{
	glUseProgram(ID);
}

void ResourceShader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void ResourceShader::setUniform(const char* name, data& unidata, UniformType UniType) const
{
	GLint uniformLoc = glGetUniformLocation(ID, "name");


	switch (UniType)
	{
	case UniformType::intU:
		glUniform1i(glGetUniformLocation(ID, name), unidata.intU);

		break;
	case UniformType::floatU:
		glUniform1f(glGetUniformLocation(ID, name), unidata.floatU);

		break;
	case UniformType::vec2U:
		glUniform2f(glGetUniformLocation(ID, name), unidata.vec2U.x, unidata.vec2U.y);

		break;
	case UniformType::vec3U:
		glUniform3f(glGetUniformLocation(ID, name), unidata.vec3U.x, unidata.vec3U.y, unidata.vec3U.z);

		break;
	case UniformType::vec4U:
		glUniform4f(glGetUniformLocation(ID, name), unidata.vec4U.x, unidata.vec4U.y, unidata.vec4U.z, unidata.vec4U.w);

		break;

	default:
		break;
	}
}