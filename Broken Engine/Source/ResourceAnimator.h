#ifndef __RESOURCE_ANIMATOR_H__
#define __RESOURCE_ANIMATOR_H__

#include "Resource.h"

BE_BEGIN_NAMESPACE

struct Animation;

class BROKEN_API ResourceAnimator : public Resource {
public:
	ResourceAnimator(uint UID, const char* source_file);
	~ResourceAnimator();

	bool LoadInMemory() override;
	void FreeMemory() override;
	
public:
	std::vector<Animation*> animations;

private:
	void OnOverwrite() override;
	void OnDelete() override;
	void Repath() override;
};
BE_END_NAMESPACE
#endif //__RESOURCE_ANIMATOR_H__