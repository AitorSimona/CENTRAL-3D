#ifndef __RESOURCE_MATERIAL_H__
#define __RESOURCE_MATERIAL_H__

#include "Resource.h"
#include "Globals.h"

#include "ResourceTexture.h"
#include "ResourceShader.h"


class ResourceMaterial : public Resource
{
public:
	ResourceMaterial();
	~ResourceMaterial();

public:
	ResourceTexture* resource_diffuse = nullptr;
	ResourceShader* shader = nullptr;
};

#endif //__RESOURCE_MATERIAL_H__