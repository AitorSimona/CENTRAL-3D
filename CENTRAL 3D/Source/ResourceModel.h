#ifndef __RESOURCE_MODEL_H__
#define __RESOURCE_MODEL_H__

#include "Resource.h"

class ResourceModel : public Resource
{
public:
	ResourceModel(uint UID, std::string source_file);
	~ResourceModel();

	bool LoadInMemory() override;
	void FreeMemory() override;
};

#endif //__RESOURCE_MODEL_H__