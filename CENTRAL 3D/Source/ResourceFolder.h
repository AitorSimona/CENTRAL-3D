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
private:
	std::vector<Resource*> resources;
};

#endif //__RESOURCE_FOLDER_H__