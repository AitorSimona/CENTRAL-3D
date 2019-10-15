#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleTextures.h"
#include "OpenGL.h"

#include "Assimp/include/scene.h"

#include "mmgr/mmgr.h"

ComponentMaterial::ComponentMaterial(Component::ComponentType type) : Component(type)
{

}

ComponentMaterial::~ComponentMaterial()
{	
	glDeleteBuffers(1, (GLuint*)&TextureID);
}

void ComponentMaterial::CreateEmpty()
{

}

// --- Create Material from Scene and path to file ---
void ComponentMaterial::CreateFromScene(const aiScene& scene, const char& file_path)
{
	// --- Get Directory from filename ---
	std::string directory = &file_path;
	App->fs->GetDirectoryFromPath(directory);

	if (scene.HasMaterials())
	{
		// --- Get scene's first material ---
		aiMaterial* material = scene.mMaterials[0];

		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString Texture_path;

			// --- Specify type of texture to retrieve (in this case DIFFUSE/ALBEDO)---
			material->GetTexture(aiTextureType_DIFFUSE, 0, &Texture_path);

			// --- Build whole path to texture file ---
			directory.append(Texture_path.C_Str());

			// --- If we find the texture file, load it ---
			if (App->fs->Exists(directory.data()))
				TextureID = App->textures->CreateTextureFromFile(directory.data());

		}
	}

}
