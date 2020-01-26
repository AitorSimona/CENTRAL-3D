#ifndef __RESOURCE_FOLDER_H__
#define __RESOURCE_FOLDER_H__

#include "Resource.h"

class ResourceFolder : public Resource
{
public:
	ResourceFolder(uint UID, std::string source_file, std::string destination_file);
	~ResourceFolder();
};

#endif //__RESOURCE_FOLDER_H__