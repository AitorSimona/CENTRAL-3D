#ifndef __RESOURCESCRIPT_H__
#define __RESOURCESCRIPT_H__

#include "Resource.h"

BE_BEGIN_NAMESPACE
class BROKEN_API ResourceScript : public Resource {
public:
	ResourceScript(uint uid, const char* source_file);
	~ResourceScript();

public:
	bool LoadInMemory() override;
	void OnOverwrite() override;
	void OnDelete() override;
	void FreeMemory() override;
public:
	std::string absolute_path;
	std::string relative_path;
	std::string script_name;

};
BE_END_NAMESPACE
#endif // !__RESOURCESCRIPT_H__

