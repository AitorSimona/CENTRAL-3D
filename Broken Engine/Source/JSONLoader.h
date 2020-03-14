#ifndef __JSONLoader_H__
#define __JSONLoader_H__

#include "BrokenCore.h"
#include "json/json.hpp"

BE_BEGIN_NAMESPACE
// for convenience
using json = nlohmann::json;

class BROKEN_API JSONLoader {
public:

	JSONLoader() {}
	~JSONLoader() {}

	json Load(const char* File) const;

	bool Save(const char* File, const json& jsonfile);

	void Serialize(const json& jsonfile, std::string& jsonserialized);

	
};
BE_END_NAMESPACE
#endif
