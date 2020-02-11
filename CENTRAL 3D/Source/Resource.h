#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <string>
#include "Globals.h"


class Resource 
{

public:
	enum class ResourceType
	{
		FOLDER,
		SCENE,
		MODEL,
		MATERIAL,
		SHADER,
		MESH,
		TEXTURE,
		SHADER_OBJECT,
		META,
		UNKNOWN,
	};

	Resource(ResourceType type);
	Resource(ResourceType type, uint UID, std::string source_file);
	virtual ~Resource();

	// --- Getters ---
	uint GetUID() const;
	void SetUID(uint UID);
	ResourceType GetType() const;
	const char* GetOriginalFile() const;
	const char* GetResourceFile() const;
	const char* GetExtension() const;
	const char* GetName() const;
	const uint GetPreviewTexID() const;
	const uint GetNumInstances() const;

	void SetOriginalFile(const char* new_path);
	void SetParent(Resource* resource);

	bool IsInMemory() const;
	bool LoadToMemory();
	void Release();

	virtual void OnOverwrite() = 0;
	virtual void OnDelete() = 0;
	virtual void OnUse() {};
	virtual void OnUnuse() {};

protected:
	// --- Utilities ---
	virtual bool LoadInMemory() = 0;
	virtual void FreeMemory() = 0;
	virtual void Repath() {};

	void SetName(const char* name);


protected:
	Resource* parent = nullptr;
	uint instances = 0;
	uint previewTexID = 0;
	uint UID = 0;
	ResourceType type = ResourceType::UNKNOWN;

	std::string resource_file = "";
	std::string original_file = "";
	std::string name = "";
	std::string extension = "";
};

#endif // !__RESOURCE_H__
