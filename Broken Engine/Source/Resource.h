#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <string>
#include <vector>
#include "BrokenCore.h"

BE_BEGIN_NAMESPACE
class GameObject;

class BROKEN_API Resource {
public:
	enum class ResourceType {
		FOLDER,
		SCENE,
		MODEL,
		PREFAB,
		MATERIAL,
		SHADER,
		MESH,
		BONE,
		ANIMATION,
		ANIMATOR,
		TEXTURE,
		SCRIPT,
		META,
		FONT,
		UNKNOWN,
	};
	enum class ResourceNotificationType {
		Overwrite,
		Deletion
	};


	Resource(ResourceType type, uint UID, const char* source_file);
	virtual ~Resource();

	// --- Getters ---
	uint GetUID() const;
	ResourceType GetType() const;
	const char* GetOriginalFile() const;
	const char* GetResourceFile() const;
	const char* GetExtension() const;
	const char* GetName() const;
	const uint GetPreviewTexID() const;
	const uint GetNumInstances() const;

	void SetOriginalFile(const char* new_path);
	void SetResourceFile(const char* new_path); // for temporal scene 
	void SetUID(uint UID);
	void SetName(const char* name);
	void SetPreviewTexID(uint ID);

	bool IsInMemory() const;
	bool LoadToMemory();
	void Release();
	void AddUser(GameObject* user);
	void RemoveUser(GameObject* user);
	void ClearUsers();
	void NotifyUsers(ResourceNotificationType type);

	virtual void OnOverwrite() = 0;
	virtual void OnDelete() = 0;
	virtual void CreateInspectorNode() {};

	// to encapsulate model childs in panelproject
	bool has_parent = false;
protected:
	// --- Utilities ---
	virtual bool LoadInMemory() = 0;
	virtual void FreeMemory() = 0;
	virtual void Repath() {};



protected:
	uint instances = 0;
	uint previewTexID = 0;
	uint UID = 0;
	ResourceType type = ResourceType::UNKNOWN;

	std::vector<GameObject*> users; // Resource notifies all interested objects of overwrites/deletes
	std::string resource_file = "";
	std::string original_file = "";
	std::string name = "";
	std::string extension = "";
};

BE_END_NAMESPACE

#endif // !__RESOURCE_H__
