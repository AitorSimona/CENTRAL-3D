#ifndef __RESOURCE_META_H__
#define __RESOURCE_META_H__

#include "Resource.h"
#include "JSONLoader.h"

class ResourceMeta : public Resource
{
public:
	ResourceMeta(uint UID, std::string source_file);
	~ResourceMeta();

	bool LoadInMemory() override;
	void FreeMemory() override;

	uint Date = 0;
	uint fileFormatVersion = 0;
	json ResourceData;

private:
	void OnOverwrite() override;
	void OnDelete() override;
	void Repath() override;
};

#endif //__RESOURCE_META_H__