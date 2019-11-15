#ifndef __RESOURCE_TEXTURE_H__
#define __RESOURCE_TEXTURE_H__

#include "Resource.h"

class ResourceTexture : public Resource
{
public:
	ResourceTexture();
	~ResourceTexture();

	void LoadOnMemory();
	void FreeMemory();

public:
	uint buffer_id = 0;
};
#endif //__RESOURCE_TEXTURE_H__
