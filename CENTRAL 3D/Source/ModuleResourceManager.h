#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include "Module.h"
#include "Resource.h"
#include "Importer.h"

class ResourceFolder;
class ResourceFolder;
class ResourceScene;
class ResourceModel;
class ResourceMaterial; 
class ResourceShader;
class ResourceMesh;
class ResourceTexture;
class ResourceMeta;
class ResourcePrefab;

class ModuleResourceManager : public Module
{
	friend class ImporterTexture;
	friend class ImporterModel;
	friend class ImporterMeta;
	friend class ImporterMesh;
	friend class ImporterMaterial;
	friend class ImporterFolder;
	friend class ImporterScene;
	friend class ImporterShader;
	friend class ImporterPrefab;
	friend class PanelResources;
	friend class ComponentMeshRenderer;
	friend class ResourceMaterial;
public:

	// --- Basic ---
	ModuleResourceManager(bool start_enabled = true);
	~ModuleResourceManager();

	bool Init(json file) override;
	bool Start() override;
	//void ONEvent(const Event& event) const override;
	update_status Update(float dt) override;
	bool CleanUp() override;

public:

	// --- Importing ---
	std::string DuplicateIntoGivenFolder(const char* path, const char* folder_path);
	ResourceFolder* SearchAssets(ResourceFolder* parent, const char* directory, std::vector<std::string>& filters);
	Resource* ImportAssets(Importer::ImportData& IData);
	Resource* ImportFolder(Importer::ImportData& IData);
	Resource* ImportScene(Importer::ImportData& IData);
	Resource* ImportModel(Importer::ImportData& IData);
	Resource* ImportPrefab(Importer::ImportData& IData);
	Resource* ImportMaterial(Importer::ImportData& IData);
	Resource* ImportMesh(Importer::ImportData& IData);
	Resource* ImportTexture(Importer::ImportData& IData);
	Resource* ImportShader(Importer::ImportData& IData);
	Resource* ImportMeta(Importer::ImportData& IData);

	void HandleFsChanges();
	void RetrieveFilesAndDirectories(const char* directory, std::map<std::string,std::vector<std::string>> & ret);

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
	Resource* GetResource(uint UID, bool loadinmemory = true);
	void AddResourceToFolder(Resource* resource);
	void RemoveResourceFromFolder(Resource* resource);
	Resource* CreateResource(Resource::ResourceType type, std::string source_file);
	Resource* CreateResourceGivenUID(Resource::ResourceType type, std::string source_file, uint UID);
	Resource::ResourceType GetResourceTypeFromPath(const char* path);
	std::string GetNewUniqueName(Resource::ResourceType type);
	bool IsFileImported(const char* file);
	void SaveResource(Resource* resource);

	void ONResourceDestroyed(Resource* resource);

	// --- Getters ---
	ResourceFolder* GetAssetsFolder();
	uint GetFileFormatVersion();
	uint GetDefaultMaterialUID();

private:

	// --- Available importers ---
	std::vector<Importer*> importers;
	std::vector<std::string> filters;

	uint fileFormatVersion = 2;

	// Use this pointers only for read ops! If you want to get the resource use GetResource function
	ResourceFolder* AssetsFolder = nullptr;
	ResourceMaterial* DefaultMaterial = nullptr;

	// --- Available resources ---
	std::map<uint, ResourceFolder*> folders;
	std::map<uint, ResourceScene*> scenes;
	std::map<uint, ResourceModel*> models;
	std::map<uint, ResourcePrefab*> prefabs;
	std::map<uint, ResourceMaterial*> materials;
	std::map<uint, ResourceShader*> shaders;
	std::map<uint, ResourceMesh*> meshes;
	std::map<uint, ResourceTexture*> textures;
	std::map<uint, ResourceMeta*> metas;
};

#endif