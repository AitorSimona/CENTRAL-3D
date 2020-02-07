#include "ImporterMaterial.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleTextures.h"
#include "ModuleResources.h"

#include "ResourceMaterial.h"
#include "ResourceFolder.h"

#include "ImporterMeta.h"
#include "ResourceMeta.h"

#include "ImporterTexture.h"

#include "Assimp/include/scene.h"

#include "mmgr/mmgr.h"

ImporterMaterial::ImporterMaterial() : Importer(Importer::ImporterType::Material)
{
}

ImporterMaterial::~ImporterMaterial()
{
}

// --- Create Material from Scene and path to file ---
Resource* ImporterMaterial::Import(ImportData& IData) const
{
	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();

	ImportMaterialData* MatData = (ImportMaterialData*)&IData;

	// --- Get Directory from filename ---
	std::string directory = MatData->path;
	directory = App->fs->GetDirectoryFromPath(directory);

	aiString Texture_relative_path;
	aiString material_name;
	std::string Texture_path;

	// --- Get material's name ---
	MatData->mat->Get(AI_MATKEY_NAME, material_name);
	ResourceMaterial* resource_mat = (ResourceMaterial*)App->resources->CreateResource(Resource::ResourceType::MATERIAL, std::string(ASSETS_FOLDER).append(material_name.C_Str()));

	// --- Get number of Diffuse textures ---
	uint num_diffuse = MatData->mat->GetTextureCount(aiTextureType_DIFFUSE);

	// --- Import them ---
	while (num_diffuse > 0)
	{
		MatData->mat->GetTexture(aiTextureType_DIFFUSE, num_diffuse - 1, &Texture_relative_path);

		// --- First actual retrieve texture path ---
		Texture_path = directory + Texture_relative_path.C_Str();

		// --- Duplicate into Assets folder ---
		std::string Assets_path = ASSETS_FOLDER;
		Assets_path.append(Texture_relative_path.C_Str());

		if (!App->fs->Exists(Assets_path.c_str()))
				App->fs->CopyFromOutsideFS(Texture_path.c_str(), Assets_path.c_str());

		// --- Finally ask resource manager to import texture ---
		ImportData TexData(Assets_path.c_str());
		ImporterTexture* ITex = App->resources->GetImporter<ImporterTexture>();

		if (ITex)
				resource_mat->resource_diffuse = (ResourceTexture*)ITex->Import(TexData);
		
		// MYTODO: Note we are only assigning one diffuse, and not caring about other texture types, create vector to store texture pointers


		num_diffuse--;
	}

	// --- Create meta ---

	ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, resource_mat->GetResourceFile(), resource_mat->GetUID());

	if (meta)
		IMeta->Save(meta);

	// --- Save data ---
	Save(resource_mat);

	// --- Add resource to relevant folder, some resources need to do it at the end of import since they do not go through ImportAssets, due to being part of a bigger resource (MODEL...) ---
	App->resources->AddResourceToFolder(resource_mat);

	return resource_mat;
}

Resource* ImporterMaterial::Load(const char * path) const
{
	Resource* mat = nullptr;

	json file = App->GetJLoader()->Load(path);

	if (!file.is_null())
	{
		std::string texture_path = file["ResourceDiffuse"];
		Importer::ImportData IData(texture_path.c_str());
		App->resources->ImportAssets(IData);
	}

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(path);

	// --- Extract data from .mat file (json) and create mat ---
	mat = App->resources->CreateResourceGivenUID(Resource::ResourceType::MATERIAL, meta->GetOriginalFile(), meta->GetUID());

	return mat;
}

void ImporterMaterial::Save(ResourceMaterial* mat) const
{
	json file;

	file[mat->GetName()];
	file["ResourceDiffuse"] = mat->resource_diffuse->GetOriginalFile();

	// --- Serialize JSON to string ---
	std::string data;
	data = App->GetJLoader()->Serialize(file);

	// --- Finally Save to file ---
	char* buffer = (char*)data.data();
	uint size = data.length();

	App->fs->Save(mat->GetResourceFile(), buffer, size);
}
