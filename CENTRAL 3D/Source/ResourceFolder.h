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
	void CreateInspectorNode() override;

	void AddResource(Resource* resource);
	void RemoveResource(Resource* resource);
	void AddChild(ResourceFolder* resource);
	void RemoveChild(ResourceFolder* child);
	std::vector<Resource*>& GetResources();
	std::vector<ResourceFolder*>& GetChilds();
	ResourceFolder* GetParent() const;

private:
	bool HasResource(Resource* resource);
	bool HasChild(ResourceFolder* resource);

	std::vector<Resource*> resources;
	std::vector<ResourceFolder*> childs;
	ResourceFolder* parent = nullptr;

private:
	void OnOverwrite() override;
	void OnDelete() override;
};

#endif //__RESOURCE_FOLDER_H__