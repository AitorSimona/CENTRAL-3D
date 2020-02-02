#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include "Module.h"
#include "Resource.h"

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
class ResourceMeta;

class ModuleResources : public Module
{
public:

	// --- Basic ---
	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	bool Init(json file) override;
	bool Start() override;
	//void ONEvent(const Event& event) const override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	// --- Importing ---
	//DEPRECATED
	void ImportAllFolders(const char* directory);

	ResourceFolder* SearchAssets(ResourceFolder* parent, const char* directory, std::vector<std::string>& filters);
	Resource* ImportAssets(const char* path);
	Resource* ImportFolder(const char* path);
	Resource* ImportScene(const char* path);
	Resource* ImportModel(const char* path);
	Resource* ImportMaterial(const char* path);
	Resource* ImportShaderProgram(const char* path);
	Resource* ImportTexture(const char* path);
	Resource* ImportShaderObject(const char* path);

	// For consistency, use this only on resource manager/importers 
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

	// --- Resource Handling ---
	Resource* CreateResource(Resource::ResourceType type, std::string source_file);
	Resource* CreateResourceGivenUID(Resource::ResourceType type, std::string source_file, uint UID);
	Resource::ResourceType GetResourceTypeFromPath(const char* path);
	ResourceFolder* GetAssetsFolder();
	const std::map<uint, ResourceFolder*>& GetAllFolders() const;
	bool IsFileImported(const char* file);

	void ONResourceDestroyed(Resource* resource);

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

	ResourceFolder* AssetsFolder = nullptr;

	// --- Available resources ---
	std::map<uint, ResourceFolder*> folders;
	std::map<uint, ResourceScene*> scenes;
	std::map<uint, ResourceModel*> models;
	std::map<uint, ResourceMaterial*> materials;
	std::map<uint, ResourceShader*> shaders;
	std::map<uint, ResourceMesh*> meshes;
	std::map<uint, ResourceTexture*> textures;
	std::map<uint, ResourceShaderObject*> shader_objects;
	std::map<uint, ResourceMeta*> metas;
};

#endif