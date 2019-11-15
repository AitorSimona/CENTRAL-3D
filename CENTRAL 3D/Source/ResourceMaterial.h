#ifndef __RESOURCE_MATERIAL_H__
#define __RESOURCE_MATERIAL_H__

#include "Resource.h"
#include "Globals.h"


class ResourceMaterial : public Resource
{
public:
	ResourceMaterial();
	~ResourceMaterial();

public:
	uint  TextureID = 0;
};

#endif //__RESOURCE_MATERIAL_H__