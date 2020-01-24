#ifndef __RESOURCE_FOLDER_H__
#define __RESOURCE_FOLDER_H__

#include "Resource.h"

class ResourceFolder : public Resource
{
public:
	ResourceFolder();
	ResourceFolder(uint UID);
	~ResourceFolder();
};

#endif //__RESOURCE_FOLDER_H__