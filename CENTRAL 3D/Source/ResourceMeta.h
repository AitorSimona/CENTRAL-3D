#ifndef __RESOURCE_META_H__
#define __RESOURCE_META_H__

#include "Resource.h"

class ResourceMeta : public Resource
{
public:
	ResourceMeta(uint UID, std::string source_file, std::string destination_file);
	~ResourceMeta();

	uint Date = 0;
};

#endif //__RESOURCE_META_H__