#ifndef __JSONLoader_H__
#define __JSONLoader_H__

#include "Globals.h"
#include "Source/json/json.hpp"


class JSONLoader
{
public:

	JSONLoader() {}
	~JSONLoader() {}

	bool Load(const char* importFile);

	bool Save(const char* File);

	bool done = false;

};

#endif
