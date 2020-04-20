#ifndef __RESOURCE_MATERIAL_H__
#define __RESOURCE_MATERIAL_H__

#include "Resource.h"
#include "Math.h"

BE_BEGIN_NAMESPACE

class ResourceTexture;
class ResourceShader;
struct Uniform;

class BROKEN_API ResourceMaterial : public Resource 
{
public:
	ResourceMaterial(uint UID, const char* source_file);
	~ResourceMaterial();

	bool LoadInMemory() override;
	void FreeMemory() override;
	void CreateInspectorNode() override;

	void UpdateUniforms();
	void DisplayAndUpdateUniforms();

	std::string previewTexPath;
public:

	bool has_transparencies = false;
	bool has_culling = true;
	float m_Shininess = 1.5f;
	float4 m_AmbientColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	ResourceTexture* m_DiffuseResTexture = nullptr;
	ResourceTexture* m_SpecularResTexture = nullptr;
	ResourceTexture* m_NormalResTexture = nullptr;

	bool m_UseTexture = true;

	ResourceShader* shader = nullptr;
	std::vector<Uniform*> uniforms;

private:

	void OnOverwrite() override;
	void OnDelete() override;
	void Repath() override;
};
BE_END_NAMESPACE
#endif //__RESOURCE_MATERIAL_H__