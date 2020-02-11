#ifndef __RESOURCE_MATERIAL_H__
#define __RESOURCE_MATERIAL_H__

#include "Resource.h"
#include "Globals.h"

#include "ResourceTexture.h"
#include "ResourceShader.h"


class ResourceMaterial : public Resource
{
public:
	ResourceMaterial(uint UID, std::string source_file);
	~ResourceMaterial();

	bool LoadInMemory() override;
	void FreeMemory() override;

	void UpdateUniforms();

public:
	ResourceTexture* resource_diffuse = nullptr;
	ResourceShader* shader = nullptr;
	std::vector<Uniform*> uniforms;

private:
	void OnOverwrite() override;
	void OnDelete() override;
	void Repath() override;
	void OnUse() override;
	void OnUnuse() override;
};

#endif //__RESOURCE_MATERIAL_H__