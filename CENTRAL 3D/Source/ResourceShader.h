#ifndef __RESOURCE_SHADER_H__
#define __RESOURCE_SHADER_H__

#include "Resource.h"

class ResourceShader : public Resource
{
public:
	ResourceShader();
	~ResourceShader();

	void LoadInMemory();
	void FreeMemory();

public:

};
#endif //__RESOURCE_SHADER_H__
