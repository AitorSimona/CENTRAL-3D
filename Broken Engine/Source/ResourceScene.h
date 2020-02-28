#ifndef __RESOURCE_SCENE_H__
#define __RESOURCE_SCENE_H__

#include "Resource.h"
#include <unordered_map>

class GameObject;

class ResourceScene : public Resource
{
public:
	ResourceScene(uint UID, std::string source_file);
	~ResourceScene();

	bool LoadInMemory() override;
	void FreeMemory() override;

	GameObject* GetGOWithName(const char* GO_name);

	std::unordered_map<uint,GameObject*> NoStaticGameObjects;
	std::unordered_map<uint, GameObject*> StaticGameObjects;

private:
	void OnOverwrite() override;
	void OnDelete() override;
};

#endif //__RESOURCE_SCENE_H__