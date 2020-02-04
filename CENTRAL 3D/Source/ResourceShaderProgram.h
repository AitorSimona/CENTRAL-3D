#ifndef __RESOURCE_SHADER_PROGRAM_H__
#define __RESOURCE_SHADER_PROGRAM_H__

#include "Resource.h"

class ResourceShaderProgram : public Resource
{
public:
	ResourceShaderProgram(uint UID, std::string source_file);
	~ResourceShaderProgram();

	bool LoadInMemory() override;
	void FreeMemory() override;
};

#endif //__RESOURCE_SHADER_PROGRAM_H__