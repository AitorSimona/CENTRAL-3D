#ifndef __RESOURCE_META_H__
#define __RESOURCE_META_H__

#include "Resource.h"

BE_BEGIN_NAMESPACE
class BROKEN_API ResourceMeta : public Resource {
public:
	ResourceMeta(uint UID, std::string source_file);
	~ResourceMeta();

	bool LoadInMemory() override;
	void FreeMemory() override;

	uint Date = 0;

private:
	void OnOverwrite() override;
	void OnDelete() override;
	void Repath() override;
};
BE_END_NAMESPACE
#endif //__RESOURCE_META_H__