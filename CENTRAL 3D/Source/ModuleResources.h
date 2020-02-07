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

public:

	// --- Importing ---
	std::string DuplicateIntoAssetsFolder(const char* path);
	ResourceFolder* SearchAssets(ResourceFolder* parent, const char* directory, std::vector<std::string>& filters);
	Resource* ImportAssets(Importer::ImportData& IData);
	Resource* ImportFolder(Importer::ImportData& IData);
	Resource* ImportScene(Importer::ImportData& IData);
	Resource* ImportModel(Importer::ImportData& IData);
	Resource* ImportMaterial(Importer::ImportData& IData);
	Resource* ImportShaderProgram(Importer::ImportData& IData);
	Resource* ImportMesh(Importer::ImportData& IData);
	Resource* ImportTexture(Importer::ImportData& IData);
	Resource* ImportShaderObject(Importer::ImportData& IData);

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

	// --- Loading ---
	Resource* GetResource(uint UID);

	// --- Resource Handling ---
	Resource* CreateResource(Resource::ResourceType type, std::string source_file);
	Resource* CreateResourceGivenUID(Resource::ResourceType type, std::string source_file, uint UID);
	Resource::ResourceType GetResourceTypeFromPath(const char* path);
	bool IsFileImported(const char* file);

	void ONResourceDestroyed(Resource* resource);

	// --- Getters ---
	ResourceFolder* GetAssetsFolder();
	uint GetDefaultMaterialUID();
	const std::map<uint, ResourceFolder*>& GetAllFolders() const;

	void AddResourceToFolder(Resource* resource);
private:

	// --- Available importers ---
	std::vector<Importer*> importers;

	// Use this pointers only for read ops! If you want to get the resource use GetResource function
	ResourceFolder* AssetsFolder = nullptr;
	ResourceMaterial* DefaultMaterial = nullptr;

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