#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include "Module.h"
#include "Resource.h"
#include "Importer.h"

class PanelBuild;
class PanelResources;

BE_BEGIN_NAMESPACE

class ResourceFolder;
class ResourceScene;
class ResourceModel;
class ResourceMaterial;
class ResourceShader;
class ResourceMesh;
class ResourceBone;
class ResourceAnimation;
class ResourceAnimator;
class ResourceTexture;
class ResourceMeta;
class ResourceScript;
class ResourceFont;
class ResourceNavMesh;
class ResourcePrefab;

class BROKEN_API ModuleResourceManager : public Module {
	friend class ImporterTexture;
	friend class ImporterModel;
	friend class ImporterMeta;
	friend class ImporterMesh;
	friend class ImporterBone;
	friend class ImporterAnimation;
	friend class ImporterAnimator;
	friend class ImporterMaterial;
	friend class ImporterScene;
	friend class ImporterFolder;
	friend class ImporterScript;
	friend class ImporterFont;
	friend class ImporterNavMesh;
	friend class ImporterShader;
	friend class ImporterPrefab;
	friend class PanelResources;
	friend class PanelBuild;
	friend class ComponentMeshRenderer;
	friend class ResourceMaterial;
	friend class ModuleSceneManager;
	friend class ModuleRenderer3D;
	friend class ScriptingScenes;
public:

	// --- Basic ---
	ModuleResourceManager(bool start_enabled = true);
	~ModuleResourceManager();

	bool Init(json& file) override;
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
	Resource* ImportBone(Importer::ImportData& IData);
	Resource* ImportAnimation(Importer::ImportData& IData);
	Resource* ImportAnimator(Importer::ImportData& IData);
	Resource* ImportTexture(Importer::ImportData& IData);
	Resource* ImportShader(Importer::ImportData& IData);
	Resource* ImportScript(Importer::ImportData& IData);
	Resource* ImportMeta(Importer::ImportData& IData);
	Resource* ImportFont(Importer::ImportData& IData);
	Resource* ImportNavMesh(Importer::ImportData& IData);

	void HandleFsChanges();
	void RetrieveFilesAndDirectories(const char* directory, std::map<std::string, std::vector<std::string>>& ret);

	// For consistency, use this only on resource manager/importers 
	template<typename TImporter>
	TImporter* GetImporter() {
		for (uint i = 0; i < importers.size(); ++i) {
			if (importers[i]->GetType() == TImporter::GetType()) {
				return ((TImporter*)(importers[i]));
			}
		}

		return nullptr;
	}

	// --- Resource Handling ---
	Resource* GetResource(uint UID, bool loadinmemory = true); 
	void AddResourceToFolder(Resource* resource);
	void RemoveResourceFromFolder(Resource* resource);
	Resource* CreateResource(Resource::ResourceType type, const char* source_file);
	Resource* CreateResourceGivenUID(Resource::ResourceType type, const char* source_file, uint UID);
	Resource::ResourceType GetResourceTypeFromPath(const char* path);
	bool IsFileImported(const char* file);
	std::shared_ptr<std::string> GetNewUniqueName(Resource::ResourceType type);
	void ONResourceDestroyed(Resource* resource);
	void ForceDelete(Resource* resource); // to prevent problems with different heaps, related to exe - dll relationship

	//MYTODO For editor panel to set currentDirectory
	void setCurrentDirectory(ResourceFolder* dir);
	ResourceFolder* getCurrentDirectory() const;

	// --- Getters ---
	ResourceFolder* GetAssetsFolder();
	uint GetFileFormatVersion();
	uint GetDefaultMaterialUID();

public:
	// OSCAR TODO try to make it private
	ResourceFont* DefaultFont = nullptr;

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
	std::map<uint, ResourceBone*> bones;
	std::map<uint, ResourceAnimation*> animations;
	std::map<uint, ResourceAnimator*> anim_info;
	std::map<uint, ResourceTexture*> textures;
	std::map<uint, ResourceScript*> scripts;
	std::map<uint, ResourceMeta*> metas;
	std::map<uint, ResourceFont*> fonts;
	std::map<uint, ResourceNavMesh*> navmeshes;

	//MYTODO Separate things needed for editor from things necessary (reading assets already imported)
	ResourceFolder* currentDirectory;
};
BE_END_NAMESPACE
#endif