#ifndef __JSONLoader_H__
#define __JSONLoader_H__

#include "Globals.h"
#include "Source/json/json.hpp"

// for convenience
using json = nlohmann::json;

class JSONLoader
{
public:

	JSONLoader() {}
	~JSONLoader() {}

	json Load(const char* File) const;

	bool Save(const char* File, json jsonfile);

};

#endif
