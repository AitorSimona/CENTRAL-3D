#ifndef __RESOURCE_SHADER_PROGRAM_H__
#define __RESOURCE_SHADER_PROGRAM_H__

#include "Resource.h"

class ResourceShaderProgram : public Resource
{
public:
	ResourceShaderProgram(uint UID, std::string source_file, std::string destination_file);
	~ResourceShaderProgram();
};

#endif //__RESOURCE_SHADER_PROGRAM_H__