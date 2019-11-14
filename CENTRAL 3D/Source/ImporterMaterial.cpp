#include "ImporterMaterial.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleTextures.h"

#include "ComponentMaterial.h"

#include "Assimp/include/scene.h"

#include "mmgr/mmgr.h"

ImporterMaterial::ImporterMaterial()
{
}

ImporterMaterial::~ImporterMaterial()
{
}

// --- Create Material from Scene and path to file ---
bool ImporterMaterial::Import(const char * File_path, const ImportData & IData) const
{
	ImportMaterialData MData = (ImportMaterialData&) IData;

	// --- Get Directory from filename ---
	std::string directory = File_path;
	App->fs->GetDirectoryFromPath(directory);

	if (MData.scene->HasMaterials())
	{
		// --- Get scene's first material ---
		aiMaterial* material = MData.scene->mMaterials[0];

		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString Texture_path;

			// --- Specify type of texture to retrieve (in this case DIFFUSE/ALBEDO)---
			material->GetTexture(aiTextureType_DIFFUSE, 0, &Texture_path);

			// --- Build whole path to texture file ---
			directory.append(Texture_path.C_Str());

			// --- If we find the texture file, load it ---
			//if (App->fs->Exists(directory.data()))
			//{
				MData.new_material->TextureID = App->textures->CreateTextureFromFile(directory.data(), MData.new_material->Texture_width, MData.new_material->Texture_height, MData.new_material->LibUID);
				MData.new_material->Texture_path = directory.data();
			//}

		}
	}

	return true;
}

void ImporterMaterial::Load(const char * filename, ComponentMaterial& mat)
{
	// --- Load from Library ---
	// --- LibUID won't be overwritten since we are loading from library ---
	mat.TextureID = App->textures->CreateTextureFromFile(filename,mat.Texture_width, mat.Texture_height, mat.LibUID,true);
	mat.Texture_path = filename;
}
