#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "Globals.h"

class Resource
{
public:
	enum ResourceType
	{
		none = 0,
		mesh
	};

public:

	Resource(Resource::ResourceType type);
	virtual ~Resource();

};

#endif

