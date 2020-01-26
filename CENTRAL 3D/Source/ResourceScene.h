#ifndef __RESOURCE_SCENE_H__
#define __RESOURCE_SCENE_H__

#include "Resource.h"

class ResourceScene : public Resource
{
public:
	ResourceScene(uint UID, std::string source_file, std::string destination_file);
	~ResourceScene();
};

#endif //__RESOURCE_SCENE_H__