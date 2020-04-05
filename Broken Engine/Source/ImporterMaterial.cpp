#include "ImporterMaterial.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleTextures.h"
#include "ModuleResourceManager.h"

#include "ResourceMaterial.h"
#include "ResourceFolder.h"

#include "ImporterMeta.h"
#include "ResourceMeta.h"

#include "ResourceShader.h"

#include "ImporterTexture.h"

#include "Assimp/include/scene.h"

#include "mmgr/mmgr.h"

using namespace Broken;
ImporterMaterial::ImporterMaterial() : Importer(Importer::ImporterType::Material) {
}

ImporterMaterial::~ImporterMaterial() {
}

// --- Create Material from Scene and path to file ---

Resource* ImporterMaterial::Import(ImportData& IData) const
{
	ImportMaterialData* MatData = (ImportMaterialData*)&IData;

	// --- Get Directory from filename ---
	std::string directory = MatData->path;
	directory = App->fs->GetDirectoryFromPath(directory);

	//aiString Texture_relative_path;
	aiString material_name;
	//std::string Texture_path;

	// --- Get material's name ---
	MatData->mat->Get(AI_MATKEY_NAME, material_name);
	ResourceMaterial* resource_mat = (ResourceMaterial*)App->resources->CreateResource(Resource::ResourceType::MATERIAL, std::string(ASSETS_FOLDER).append(material_name.C_Str()).append(".mat").c_str());

	//// --- Get number of Diffuse textures ---
	//uint num_diffuse = MatData->mat->GetTextureCount(aiTextureType_DIFFUSE);

	//// --- Import them ---
	//while (num_diffuse > 0)
	//{
	//	MatData->mat->GetTexture(aiTextureType_DIFFUSE, num_diffuse - 1, &Texture_relative_path);

	//	// --- First actual retrieve texture path ---
	//	Texture_path = directory + Texture_relative_path.C_Str();

	//	// --- Duplicate into Assets folder ---
	//	std::string Assets_path = ASSETS_FOLDER;
	//	Assets_path.append(Texture_relative_path.C_Str());

	//	if (!App->fs->Exists(Assets_path.c_str()))
	//			App->fs->CopyFromOutsideFS(Texture_path.c_str(), Assets_path.c_str());

	//	// --- Finally ask resource manager to import texture ---
	//	ImportData TexData(Assets_path.c_str());
	//	ImporterTexture* ITex = App->resources->GetImporter<ImporterTexture>();

	//	if (ITex)
	//			resource_mat->resource_diffuse = (ResourceTexture*)ITex->Import(TexData);

	//	if (resource_mat->resource_diffuse)
	//		resource_mat->resource_diffuse->SetParent(resource_mat);
	//
	//	// MYTODO: Note we are only assigning one diffuse, and not caring about other texture types, create vector to store texture pointers


	//	num_diffuse--;
	//}


	// --- Save data ---
	Save(resource_mat);

	// --- Add resource to relevant folder, some resources need to do it at the end of import since they do not go through ImportAssets, due to being part of a bigger resource (MODEL...) ---
	App->resources->AddResourceToFolder(resource_mat);

	// --- Create meta ---
	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();

	ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, resource_mat->GetResourceFile(), resource_mat->GetUID());

	if (meta)
		IMeta->Save(meta);

	return resource_mat;
}

Resource* ImporterMaterial::Load(const char* path) const
{
	ResourceMaterial* mat = nullptr;
	ResourceTexture* diffuse = nullptr;
	ResourceTexture* specular = nullptr;
	ResourceTexture* normalMap = nullptr;
	float3 matColor = float3(1.0f);
	float matShine = 32.0f;

	json file = App->GetJLoader()->Load(path);

	std::string diffuse_texture_path = file["ResourceDiffuse"].is_null() ? "NaN.dds" : file["ResourceDiffuse"].get<std::string>();
	std::string specular_texture_path = file["ResourceSpecular"].is_null() ? "NaN.dds" : file["ResourceSpecular"].get<std::string>();
	std::string normal_texture_path = file["ResourceNormalTexture"].is_null() ? "NaN.dds" : file["ResourceNormalTexture"].get<std::string>();

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(path);

	// --- Extract data from .mat file (json) and create mat ---
	mat = App->resources->materials.find(meta->GetUID()) != App->resources->materials.end() ? App->resources->materials.find(meta->GetUID())->second : (ResourceMaterial*)App->resources->CreateResourceGivenUID(Resource::ResourceType::MATERIAL, meta->GetOriginalFile(), meta->GetUID());

	// --- A folder has been renamed ---
	if (!App->fs->Exists(mat->GetOriginalFile()))
	{
		mat->SetOriginalFile(path);
		meta->SetOriginalFile(path);
		App->resources->AddResourceToFolder(mat);
	}

	if (!file.is_null())
	{
		if (!file["AmbientColor"].is_null())
			matColor = float3(file["AmbientColor"]["R"].get<float>(), file["AmbientColor"]["G"].get<float>(), file["AmbientColor"]["B"].get<float>());

		if (!file["MaterialShininess"].is_null())
			matShine = file["MaterialShininess"].get<float>();

		Importer::ImportData IDataDiff(diffuse_texture_path.c_str());

		if(diffuse_texture_path != "NaN.dds")
			diffuse = (ResourceTexture*)App->resources->ImportAssets(IDataDiff);

		Importer::ImportData IDataSpec(specular_texture_path.c_str());

		if (specular_texture_path != "NaN.dds")
			specular = (ResourceTexture*)App->resources->ImportAssets(IDataSpec);

		Importer::ImportData IDataNormTex(normal_texture_path.c_str());
		
		if(normal_texture_path != "NaN.dds")
			normalMap = (ResourceTexture*)App->resources->ImportAssets(IDataNormTex);

		// --- Load Shader and Uniforms ---
		std::string shader_path = file["shader"]["ResourceShader"].is_null() ? "NONE" : file["shader"]["ResourceShader"].get<std::string>();
		Importer::ImportData IDataShader(shader_path.c_str());

		if(shader_path != "NONE")
			mat->shader = (ResourceShader*)App->resources->ImportAssets(IDataShader);

		json uniforms_node = file["shader"]["uniforms"];

		if (mat->shader && !uniforms_node.is_null())
		{
			App->resources->GetResource(mat->shader->GetUID());

			// --- Delete all uniforms ---
			mat->FreeMemory();

			float* tmpf = new float[4];
			int* tmpi = new int[4];

			for (json::iterator iterator = uniforms_node.begin(); iterator != uniforms_node.end(); ++iterator)
			{
				Uniform* uniform = new Uniform();
				uniforms_node[iterator.key()];
				uint unitype = uniforms_node[iterator.key()]["type"];
				std::string name = iterator.key();

				switch (unitype)
				{
				case GL_INT:
					mat->shader->FillUniform(uniform, name.c_str(), unitype);
					uniform->value.intU = uniforms_node[iterator.key()]["x"].get<int>();
					mat->shader->setUniform(name.c_str(), uniform->value, UniformType::intU);
					break;

				case GL_FLOAT:
					mat->shader->FillUniform(uniform, name.c_str(), unitype);
					uniform->value.intU = uniforms_node[iterator.key()]["x"].get<float>();
					mat->shader->setUniform(name.c_str(), uniform->value, UniformType::floatU);
					break;

				case GL_FLOAT_VEC2:
					mat->shader->FillUniform(uniform, name.c_str(), unitype);
					uniform->value.vec2U.x = uniforms_node[iterator.key()]["x"].get<float>();
					uniform->value.vec2U.y = uniforms_node[iterator.key()]["x"].get<float>();
					mat->shader->setUniform(name.c_str(), uniform->value, UniformType::vec2U);
					break;

				case GL_FLOAT_VEC3:
					mat->shader->FillUniform(uniform, name.c_str(), unitype);
					uniform->value.vec3U.x = uniforms_node[iterator.key()]["x"].get<float>();
					uniform->value.vec3U.y = uniforms_node[iterator.key()]["x"].get<float>();
					uniform->value.vec3U.z = uniforms_node[iterator.key()]["z"].get<float>();
					mat->shader->setUniform(name.c_str(), uniform->value, UniformType::vec3U);
					break;

				case GL_FLOAT_VEC4:
					mat->shader->FillUniform(uniform, name.c_str(), unitype);
					uniform->value.vec4U.x = uniforms_node[iterator.key()]["x"].get<float>();
					uniform->value.vec4U.y = uniforms_node[iterator.key()]["x"].get<float>();
					uniform->value.vec4U.z = uniforms_node[iterator.key()]["z"].get<float>();
					uniform->value.vec4U.w = uniforms_node[iterator.key()]["w"].get<float>();
					mat->shader->setUniform(name.c_str(), uniform->value, UniformType::vec4U);
					break;

				case GL_INT_VEC2:
					mat->shader->FillUniform(uniform, name.c_str(), unitype);
					uniform->value.vec2U.x = uniforms_node[iterator.key()]["x"].get<int>();
					uniform->value.vec2U.y = uniforms_node[iterator.key()]["x"].get<int>();
					mat->shader->setUniform(name.c_str(), uniform->value, UniformType::vec2U);
					break;

				case GL_INT_VEC3:
					mat->shader->FillUniform(uniform, name.c_str(), unitype);
					uniform->value.vec3U.x = uniforms_node[iterator.key()]["x"].get<int>();
					uniform->value.vec3U.y = uniforms_node[iterator.key()]["x"].get<int>();
					uniform->value.vec3U.z = uniforms_node[iterator.key()]["z"].get<int>();
					mat->shader->setUniform(name.c_str(), uniform->value, UniformType::vec3U);
					break;

				case GL_INT_VEC4:
					mat->shader->FillUniform(uniform, name.c_str(), unitype);
					uniform->value.vec4U.x = uniforms_node[iterator.key()]["x"].get<int>();
					uniform->value.vec4U.y = uniforms_node[iterator.key()]["x"].get<int>();
					uniform->value.vec4U.z = uniforms_node[iterator.key()]["z"].get<int>();
					uniform->value.vec4U.w = uniforms_node[iterator.key()]["w"].get<int>();
					mat->shader->setUniform(name.c_str(), uniform->value, UniformType::vec4U);
					break;

				default:
					continue;
					break;

				}

				mat->uniforms.push_back(uniform);
			}

			delete[] tmpf;
			delete[] tmpi;
		}
	}

	mat->m_AmbientColor = matColor;
	mat->m_Shininess = matShine;

	if (diffuse)
		mat->m_DiffuseResTexture = diffuse;	//mat->resource_diffuse->SetParent(mat);

	if (specular)
		mat->m_SpecularResTexture = specular;	//mat->resource_diffuse->SetParent(mat);

	if (normalMap)
		mat->m_NormalResTexture = normalMap;	//mat->resource_diffuse->SetParent(mat);

	return mat;
}

void ImporterMaterial::Save(ResourceMaterial* mat) const
{
	if (mat->GetUID() == App->resources->DefaultMaterial->GetUID())
		return;

	json file;

	file[mat->GetName()];
	file["ResourceDiffuse"];
	file["ResourceSpecular"];
	file["ResourceNormalTexture"];
	file["AmbientColor"];

	// --- Save Shader and Uniforms ---
	file["shader"];
	file["shader"]["ResourceShader"] = mat->shader ? std::string(mat->shader->GetOriginalFile()) : "0";
	file["shader"]["uniforms"];

	if (mat->shader)
	{
		float* tmpf = new float[4];
		int* tmpi = new int[4];

		for (std::vector<Uniform*>::const_iterator iterator = mat->uniforms.begin(); iterator != mat->uniforms.end(); ++iterator)
		{
			file["shader"]["uniforms"][(*iterator)->name];
			file["shader"]["uniforms"][(*iterator)->name]["type"] = (*iterator)->type;

			switch ((*iterator)->type)
			{
			case GL_INT:
				glGetUniformiv(mat->shader->ID, (*iterator)->location, tmpi);
				file["shader"]["uniforms"][(*iterator)->name]["x"] = tmpi[0];
				break;

			case GL_FLOAT:
				glGetUniformfv(mat->shader->ID, (*iterator)->location, tmpf);
				file["shader"]["uniforms"][(*iterator)->name]["x"] = tmpf[0];
				break;

			case GL_FLOAT_VEC2:
				glGetUniformfv(mat->shader->ID, (*iterator)->location, tmpf);
				file["shader"]["uniforms"][(*iterator)->name]["x"] = tmpf[0];
				file["shader"]["uniforms"][(*iterator)->name]["y"] = tmpf[1];
				break;

			case GL_FLOAT_VEC3:
				glGetUniformfv(mat->shader->ID, (*iterator)->location, tmpf);
				file["shader"]["uniforms"][(*iterator)->name]["x"] = tmpf[0];
				file["shader"]["uniforms"][(*iterator)->name]["y"] = tmpf[1];
				file["shader"]["uniforms"][(*iterator)->name]["z"] = tmpf[2];
				break;

			case GL_FLOAT_VEC4:
				glGetUniformfv(mat->shader->ID, (*iterator)->location, tmpf);
				file["shader"]["uniforms"][(*iterator)->name]["x"] = tmpf[0];
				file["shader"]["uniforms"][(*iterator)->name]["y"] = tmpf[1];
				file["shader"]["uniforms"][(*iterator)->name]["z"] = tmpf[2];
				file["shader"]["uniforms"][(*iterator)->name]["w"] = tmpf[3];
				break;

			case GL_INT_VEC2:
				glGetUniformiv(mat->shader->ID, (*iterator)->location, tmpi);
				file["shader"]["uniforms"][(*iterator)->name]["x"] = tmpi[0];
				file["shader"]["uniforms"][(*iterator)->name]["y"] = tmpi[1];
				break;

			case GL_INT_VEC3:
				glGetUniformiv(mat->shader->ID, (*iterator)->location, tmpi);
				file["shader"]["uniforms"][(*iterator)->name]["x"] = tmpi[0];
				file["shader"]["uniforms"][(*iterator)->name]["y"] = tmpi[1];
				file["shader"]["uniforms"][(*iterator)->name]["z"] = tmpi[2];
				break;

			case GL_INT_VEC4:
				glGetUniformiv(mat->shader->ID, (*iterator)->location, tmpi);
				file["shader"]["uniforms"][(*iterator)->name]["x"] = tmpi[0];
				file["shader"]["uniforms"][(*iterator)->name]["y"] = tmpi[1];
				file["shader"]["uniforms"][(*iterator)->name]["z"] = tmpi[2];
				file["shader"]["uniforms"][(*iterator)->name]["w"] = tmpi[3];
				break;

			default:
				continue;
				break;

			}

		}

		delete[] tmpf;
		delete[] tmpi;
	}

	file["AmbientColor"]["R"] = mat->m_AmbientColor.x;
	file["AmbientColor"]["G"] = mat->m_AmbientColor.y;
	file["AmbientColor"]["B"] = mat->m_AmbientColor.z;
	file["MaterialShininess"] = mat->m_Shininess;

	if (mat->m_DiffuseResTexture)
		file["ResourceDiffuse"] = mat->m_DiffuseResTexture->GetOriginalFile();
	if (mat->m_SpecularResTexture)
		file["ResourceSpecular"] = mat->m_SpecularResTexture->GetOriginalFile();
	if (mat->m_NormalResTexture)
		file["ResourceNormalTexture"] = mat->m_NormalResTexture->GetOriginalFile();

	// --- Serialize JSON to string ---
	std::string data;
	App->GetJLoader()->Serialize(file, data);

	// --- Finally Save to file ---
	char* buffer = (char*)data.data();
	uint size = data.length();

	App->fs->Save(mat->GetResourceFile(), buffer, size);

	// --- Update meta ---
	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(mat->GetOriginalFile());

	if (meta)
	{
		meta->Date = App->fs->GetLastModificationTime(mat->GetOriginalFile());
		IMeta->Save(meta);
	}
	else
		ENGINE_CONSOLE_LOG("|[error]: Could not load meta from: %s", mat->GetResourceFile());
}