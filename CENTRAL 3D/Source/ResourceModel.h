#ifndef __RESOURCE_MODEL_H__
#define __RESOURCE_MODEL_H__

#include "Resource.h"

class ResourceModel : public Resource
{
public:
	ResourceModel(uint UID, std::string source_file, std::string destination_file);
	~ResourceModel();
};

#endif //__RESOURCE_MODEL_H__