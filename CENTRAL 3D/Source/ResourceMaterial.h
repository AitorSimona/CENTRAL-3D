#ifndef __RESOURCE_MATERIAL_H__
#define __RESOURCE_MATERIAL_H__

#include "Resource.h"
#include "Globals.h"
#include "Color.h"

class ResourceTexture;
class ResourceShader;
struct Uniform;

class ResourceMaterial : public Resource
{
public:
	ResourceMaterial(uint UID, std::string source_file);
	~ResourceMaterial();

	bool LoadInMemory() override;
	void FreeMemory() override;
	void CreateInspectorNode() override;

	void UpdateUniforms();
	void DisplayAndUpdateUniforms();

	std::string previewTexPath;
public:
	ResourceTexture* resource_diffuse = nullptr;
	ResourceShader* shader = nullptr;
	std::vector<Uniform*> uniforms;
	Color color = White;

	bool reflective = false;
	bool refractive = false;
private:
	void OnOverwrite() override;
	void OnDelete() override;
	void Repath() override;
};

#endif //__RESOURCE_MATERIAL_H__