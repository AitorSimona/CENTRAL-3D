#ifndef __RESOURCE_MODEL_H__
#define __RESOURCE_MODEL_H__

#include "Resource.h"
#include <vector>

class Resource;

class ResourceModel : public Resource
{
public:
	ResourceModel(uint UID, std::string source_file);
	~ResourceModel();

	bool LoadInMemory() override;
	void FreeMemory() override;
	void CreateInspectorNode() override;

	void AddResource(Resource* resource);
	void RemoveResource(Resource* resource);
	std::vector<Resource*>* GetResources();

	bool openInProject = false;
	std::string previewTexPath;
private:
	std::vector<Resource*> resources;
	bool HasResource(Resource* resource);
	
private:
	void OnOverwrite() override;
	void OnDelete() override;
};

#endif //__RESOURCE_MODEL_H__