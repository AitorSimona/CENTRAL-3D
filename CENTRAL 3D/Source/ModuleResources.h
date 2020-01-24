#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include "Module.h"
#include "Resource.h"

//class ResourceShader;
//
//struct ResourceMeta
//{
//	uint Date = 0;
//	uint UID = 0;
//};

class Importer;

class ResourceFolder;
class ResourceFolder;
class ResourceScene;
class ResourceModel;
class ResourceMaterial; 
class ResourceShader;
class ResourceMesh;
class ResourceTexture;
class ResourceShaderObject;

class ModuleResources : public Module
{
public:

	// --- Basic ---
	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	bool Init(json file);
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	// --- Importing ---
	void SearchAssets(const char* directory, std::vector<std::string>& filters);
	void ImportAssets(const char* path);
	Resource* ImportFolder(const char* path);
	Resource* ImportScene(const char* path);
	Resource* ImportModel(const char* path);
	Resource* ImportMaterial(const char* path);
	Resource* ImportShaderProgram(const char* path);
	Resource* ImportTexture(const char* path);
	Resource* ImportShaderObject(const char* path);

	Resource::ResourceType GetResourceTypeFromPath(const char* path);
	uint GetUIDFromMeta(const char* file);
	bool IsFileImported(const char* file);

	template<typename TImporter>
	TImporter* GetImporter()
	{
		for (uint i = 0; i < importers.size(); ++i)
		{
			if (importers[i]->GetType() == TImporter::GetType())
			{
				return ((TImporter*)(importers[i]));
			}
		}

		return nullptr;
	}


	/*void CreateMetaFromUID(uint UID, const char* filename);

	bool IsFileImported(const char* file);
	Resource* GetResource(const char* original_file);
	Resource::ResourceType GetResourceTypeFromPath(const char* path);
	uint GetUIDFromMeta(const char* file);
	uint GetModDateFromMeta(const char* file);

	std::map<std::string, ResourceShader*>* GetShaders();
	void SaveAllShaders();


	Resource* CreateResource(Resource::ResourceType type);
	void AddResource(Resource* res);
	void AddShader(ResourceShader* shader);*/


private:
	// --- Available importers ---
	std::vector<Importer*> importers;

	// --- Available resources ---
	std::map<uint, ResourceFolder*> folders;
	std::map<uint, ResourceScene*> scenes;
	std::map<uint, ResourceModel*> models;
	std::map<uint, ResourceMaterial*> materials;
	std::map<uint, ResourceShader*> shaders;
	std::map<uint, ResourceMesh*> meshes;
	std::map<uint, ResourceTexture*> textures;
	std::map<uint, ResourceShaderObject*> shader_objects;
};

#endif