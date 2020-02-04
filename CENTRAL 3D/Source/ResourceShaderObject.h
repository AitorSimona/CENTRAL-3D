#ifndef __RESOURCE_SHADER_OBJECT_H__
#define __RESOURCE_SHADER_OBJECT_H__

#include "Resource.h"

class ResourceShaderObject : public Resource
{
public:
	ResourceShaderObject(uint UID, std::string source_file);
	~ResourceShaderObject();

	bool LoadInMemory() override;
	void FreeMemory() override;
};

#endif //__RESOURCE_SHADER_OBJECT_H__