#ifndef __RESOURCE_MATERIAL_H__
#define __RESOURCE_MATERIAL_H__

#include "Resource.h"

#include "ResourceTexture.h"
#include "ResourceShader.h"

BE_BEGIN_NAMESPACE
class BROKEN_API ResourceMaterial : public Resource {
public:
	ResourceMaterial(uint UID, const char* source_file);
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
};
BE_END_NAMESPACE
#endif //__RESOURCE_MATERIAL_H__