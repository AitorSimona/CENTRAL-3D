#ifndef __RESOURCE_FOLDER_H__
#define __RESOURCE_FOLDER_H__

#include "Resource.h"
#include <vector>

class ResourceFolder : public Resource
{
public:
	ResourceFolder();
	~ResourceFolder();

	std::vector<Resource*> GetChilds() const;

private:
	std::vector<Resource*> childs;
};

#endif //__RESOURCE_FOLDER_H__