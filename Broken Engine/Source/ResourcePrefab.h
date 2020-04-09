#ifndef __RESOURCE_PREFAB_H__
#define __RESOURCE_PREFAB_H__

#include "Resource.h"

BE_BEGIN_NAMESPACE

class ResourceModel;

class BROKEN_API ResourcePrefab : public Resource
{
public:
	ResourcePrefab(uint UID, const char* source_file);
	~ResourcePrefab();

	bool LoadInMemory() override;
	void FreeMemory() override;
	void CreateInspectorNode() override;

	void EditPrefab();

	ResourceModel* model = nullptr;
	GameObject* parentgo = nullptr;
	std::string previewTexPath;
private:
	void OnOverwrite() override;
	void OnDelete() override;
};
BE_END_NAMESPACE
#endif //__RESOURCE_PREFAB_H__
