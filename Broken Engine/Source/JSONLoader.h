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

	bool Save(const char* File, json jsonfile);

	std::string Serialize(json jsonfile);
};
BE_END_NAMESPACE
#endif
