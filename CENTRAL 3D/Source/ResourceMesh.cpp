#include "ResourceMesh.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

ResourceMesh::ResourceMesh() : Resource(Resource::ResourceType::mesh)
{
}

ResourceMesh::~ResourceMesh()
{
}

void ResourceMesh::Import(const char * path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array

		for (uint i = 0; i < scene->mNumMeshes; ++i)
		{
			// Call mesh importer
		}



		aiReleaseImport(scene);
	}
	else
		LOG("|[error]: Error loading scene %s", path);

}
