#ifndef __RESOURCE_FOLDER_H__
#define __RESOURCE_FOLDER_H__

#include "Resource.h"
#include <vector>

class ResourceFolder : public Resource
{
public:
	ResourceFolder(uint UID, std::string source_file);
	~ResourceFolder();

	bool LoadInMemory() override;
	void FreeMemory() override;

	void AddResource(Resource* resource);
	void SetParent(ResourceFolder* parent);
	std::vector<Resource*>& GetResources();
	std::vector<ResourceFolder*>& GetChilds();
	ResourceFolder* GetParent() const;

private:
	bool HasResource(Resource* resource);

	std::vector<Resource*> resources;
	std::vector<ResourceFolder*> childs;
	ResourceFolder* parent = nullptr;

private:
	void OnOverwrite() override;
	void OnDelete() override;
};

#endif //__RESOURCE_FOLDER_H__