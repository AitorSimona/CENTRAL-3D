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

private:
	std::vector<Resource*> resources;
};

#endif //__RESOURCE_MODEL_H__