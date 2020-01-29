#ifndef __RESOURCE_FOLDER_H__
#define __RESOURCE_FOLDER_H__

#include "Resource.h"
#include <vector>

class ResourceFolder : public Resource
{
public:
	ResourceFolder(uint UID, std::string source_file);
	~ResourceFolder();

	void AddResource(Resource* resource);
	void SetParent(ResourceFolder* parent);
	const std::vector<Resource*>& GetResources();
	const std::vector<ResourceFolder*>& GetChilds();
	const ResourceFolder* GetParent();


private:
	std::vector<Resource*> resources;
	std::vector<ResourceFolder*> childs;
	ResourceFolder* parent = nullptr;
};

#endif //__RESOURCE_FOLDER_H__