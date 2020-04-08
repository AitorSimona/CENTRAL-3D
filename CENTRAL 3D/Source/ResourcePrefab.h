#ifndef __RESOURCE_PREFAB_H__
#define __RESOURCE_PREFAB_H__

#include "Resource.h"

class ResourcePrefab : public Resource
{
public:
	ResourcePrefab(uint UID, std::string source_file);
	~ResourcePrefab();

	bool LoadInMemory() override;
	void FreeMemory() override;
	void CreateInspectorNode() override;

private:
	void OnOverwrite() override;
	void OnDelete() override;
};
#endif //__RESOURCE_PREFAB_H__
