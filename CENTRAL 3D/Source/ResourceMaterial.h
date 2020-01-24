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
	ResourceMaterial(uint UID);
	~ResourceMaterial();

	void UpdateUniforms();

public:
	ResourceTexture* resource_diffuse = nullptr;
	ResourceShader* shader = nullptr;
	std::vector<Uniform*> uniforms;
};

#endif //__RESOURCE_MATERIAL_H__