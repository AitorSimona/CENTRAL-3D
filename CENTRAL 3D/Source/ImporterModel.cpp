#include "ImporterModel.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleSceneManager.h"
#include "ModuleResources.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "GameObject.h"
#include "ImporterScene.h"


ImporterModel::ImporterModel() : Importer(Importer::ImporterType::Model)
{
}

ImporterModel::~ImporterModel()
{
}

// --- Import external file ---
Resource* ImporterModel::Import(ImportData* IData) const
{
	// --- Import scene from path ---
	const aiScene* model = aiImportFileEx(IData->path, aiProcessPreset_TargetRealtime_MaxQuality | aiPostProcessSteps::aiProcess_FlipUVs, App->fs->GetAssimpIO());

	GameObject* rootnode = nullptr;
	ImporterScene* IScene = App->resources->GetImporter<ImporterScene>();

	if (model && IScene)
	{
		rootnode = App->scene_manager->CreateEmptyGameObject();
		//// --- Set root node name as file name with no extension ---
		//rootnode->SetName(rootnodename.data());

		// --- Save Game objects to vector so we can save to lib later ---
		std::vector<GameObject*> model_gos;
		model_gos.push_back(rootnode);

		// --- Load all meshes ---
		std::map<uint, ResourceMesh*> model_meshes;
		IScene->LoadSceneMeshes(model, model_meshes);

		// --- Use scene->mNumMeshes to iterate on scene->mMeshes array ---
		IScene->LoadNodes(model->mRootNode,rootnode,model, model_gos, IData->path, model_meshes);

		// --- Save to Own format files in Library ---
		Save(model_gos, rootnode->GetName());

		// --- Free everything ---
		IScene->FreeSceneMeshes(&model_meshes);
		rootnode->RecursiveDelete();

		// --- Free scene ---
		aiReleaseImport(model);

	}
	else
		CONSOLE_LOG("|[error]: Error loading FBX %s", &IData->path);

	return nullptr;
}


// --- Load file from library ---
Resource* ImporterModel::Load(const char* path) const
{


	return nullptr;
}

void ImporterModel::Save(std::vector<GameObject*>& model_gos, std::string& model_name) const
{
	// --- Save Model to file ---

	json file;

	for (int i = 0; i < model_gos.size(); ++i)
	{
		// --- Create GO Structure ---
		file[model_gos[i]->GetName()];
		file[model_gos[i]->GetName()]["UID"] = std::to_string(model_gos[i]->GetUID());
		file[model_gos[i]->GetName()]["Parent"] = std::to_string(model_gos[i]->parent->GetUID());
		file[model_gos[i]->GetName()]["Components"];

		for (int j = 0; j < model_gos[i]->GetComponents().size(); ++j)
		{
			json node;
			//std::string component_path;
			//ComponentTransform* transform = transform = scene_gos[i]->GetComponent<ComponentTransform>(Component::ComponentType::Transform);
			//float3 position = transform->GetPosition();
			//float3 rotation = transform->GetRotation();
			//float3 scale = transform->GetScale();

			//ComponentMesh* mesh = scene_gos[i]->GetComponent<ComponentMesh>(Component::ComponentType::Mesh);

			//ComponentCamera* camera = scene_gos[i]->GetComponent<ComponentCamera>(Component::ComponentType::Camera);

			// --- Save Components to files ---

			switch (model_gos[i]->GetComponents()[j]->GetType())
			{

			case Component::ComponentType::Transform:
				node = file[model_gos[i]->GetName()]["Components"][std::to_string((uint)model_gos[i]->GetComponents()[j]->GetType())];
				model_gos[i]->GetComponents()[j]->Save(node);
				// --- Store path to component file ---

				//file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["positionx"] = std::to_string(position.x);
				//file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["positiony"] = std::to_string(position.y);
				//file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["positionz"] = std::to_string(position.z);

				//file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["rotationx"] = std::to_string(rotation.x);
				//file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["rotationy"] = std::to_string(rotation.y);
				//file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["rotationz"] = std::to_string(rotation.z);

				//file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["scalex"] = std::to_string(scale.x);
				//file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["scaley"] = std::to_string(scale.y);
				//file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["scalez"] = std::to_string(scale.z);

				break;
			case Component::ComponentType::Mesh:
				//component_path = MESHES_FOLDER;
				//component_path.append(std::to_string(mesh->resource_mesh->GetUID()));
				//component_path.append(".mesh");

				//if(!App->fs->Exists(component_path.data()))
				//IMesh->Save(scene_gos[i]->GetComponent<ComponentMesh>()->resource_mesh, component_path.data());

				// --- Store path to component file ---
				//file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())] = component_path;
				break;
			case Component::ComponentType::Renderer:
				// --- Store path to component file ---
				//file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())] = component_path;
				break;

			case Component::ComponentType::Material:
				//if (scene_gos[i]->GetComponent<ComponentMaterial>(Component::ComponentType::Material)->resource_material->resource_diffuse)
				//{
				//	component_path = TEXTURES_FOLDER;
				//	component_path.append(std::to_string(scene_gos[i]->GetComponent<ComponentMaterial>(Component::ComponentType::Material)->resource_material->resource_diffuse->GetUID()));
				//	component_path.append(".dds");

				//	// --- Store path to component file ---
				//	file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())];
				//	file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["diffuse"] = component_path;
				//	component_path = ((scene_gos[i]->GetComponent<ComponentMaterial>(Component::ComponentType::Material)->resource_material->shader->name));
				//	file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["shader"] = component_path;
				//	file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"];

				//	std::vector<Uniform*>* uniforms = &((scene_gos[i]->GetComponent<ComponentMaterial>(Component::ComponentType::Material)->resource_material->uniforms));
				//	uint shader = ((scene_gos[i]->GetComponent<ComponentMaterial>(Component::ComponentType::Material)->resource_material->shader->ID));

				//	float* tmpf = new float[4];
				//	int* tmpi = new int[4];


				//	for (std::vector<Uniform*>::const_iterator iterator = uniforms->begin(); iterator != uniforms->end(); ++iterator)
				//	{
				//		file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name];
				//		file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["type"] = std::to_string((*iterator)->type);

				//		switch ((*iterator)->type)
				//		{
				//		case GL_INT:				
				//			glGetUniformiv(shader, (*iterator)->location, tmpi);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["x"] = std::to_string(tmpi[0]);
				//			break;

				//		case GL_FLOAT:
				//			glGetUniformfv(shader, (*iterator)->location, tmpf);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["x"] = std::to_string(tmpf[0]);
				//			break;

				//		case GL_FLOAT_VEC2:
				//			glGetUniformfv(shader, (*iterator)->location, tmpf);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["x"] = std::to_string(tmpf[0]);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["y"] = std::to_string(tmpf[1]);
				//			break;

				//		case GL_FLOAT_VEC3:
				//			glGetUniformfv(shader, (*iterator)->location, tmpf);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["x"] = std::to_string(tmpf[0]);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["y"] = std::to_string(tmpf[1]);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["z"] = std::to_string(tmpf[2]);
				//			break;

				//		case GL_FLOAT_VEC4:
				//			glGetUniformfv(shader, (*iterator)->location, tmpf);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["x"] = std::to_string(tmpf[0]);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["y"] = std::to_string(tmpf[1]);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["z"] = std::to_string(tmpf[2]);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["w"] = std::to_string(tmpf[3]);
				//			break;

				//		case GL_INT_VEC2:
				//			glGetUniformiv(shader, (*iterator)->location, tmpi);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["x"] = std::to_string(tmpi[0]);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["y"] = std::to_string(tmpi[1]);
				//			break;

				//		case GL_INT_VEC3:
				//			glGetUniformiv(shader, (*iterator)->location, tmpi);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["x"] = std::to_string(tmpi[0]);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["y"] = std::to_string(tmpi[1]);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["z"] = std::to_string(tmpi[2]);
				//			break;

				//		case GL_INT_VEC4:
				//			glGetUniformiv(shader, (*iterator)->location, tmpi);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["x"] = std::to_string(tmpi[0]);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["y"] = std::to_string(tmpi[1]);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["z"] = std::to_string(tmpi[2]);
				//			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["uniforms"][(*iterator)->name]["w"] = std::to_string(tmpi[3]);
				//			break;

				//		default:
				//			continue;
				//			break;

				//		}

				//	}

				//	delete[] tmpf;
				//	delete[] tmpi;

				//}
				break;

			case Component::ComponentType::Camera:
				//file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["FOV"] = std::to_string(camera->GetFOV());
				//file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["NEARPLANE"] = std::to_string(camera->GetNearPlane());
				//file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["FARPLANE"] = std::to_string(camera->GetFarPlane());
				//file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())]["ASPECTRATIO"] = std::to_string(camera->GetAspectRatio());
				break;

			}


			//	}
			//}
			//// --- Serialize JSON to string ---
			//std::string data;
			//data = App->GetJLoader()->Serialize(file);

			//// --- Set destination file given exportfile type ---
			//std::string path;
			//uint new_uid;
			//std::string filename = ASSETS_FOLDER;
			//filename.append(scene_name);
			//switch (exportedfile_type)
			//{
			//	case MODEL:
			//		path = MODELS_FOLDER;
			//		new_uid = App->GetRandom().Int();
			//		path.append(std::to_string(new_uid));
			//		path.append(".model");	
			//		filename.append(".fbx");
			//		App->resources->CreateMetaFromUID(new_uid, filename.data());
			//		break;

			//	case SCENE:
			//		path = SCENES_FOLDER;
			//		path.append(scene_name);
			//		path.append(".scene");
			//		break;
			//}

			//// --- Finally Save to file ---
			//char* buffer = (char*)data.data();
			//uint size = data.length();

			//App->fs->Save(path.data(), buffer, size);
		}
	}
}

