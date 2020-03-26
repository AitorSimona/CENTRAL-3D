#ifndef __RESOURCE_META_H__
#define __RESOURCE_META_H__

#include "Resource.h"
#include "JSONLoader.h"

BE_BEGIN_NAMESPACE
class BROKEN_API ResourceMeta : public Resource {
public:
	ResourceMeta(uint UID, const char* source_file);
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
BE_END_NAMESPACE
#endif //__RESOURCE_META_H__