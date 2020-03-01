#include "ComponentMeshRenderer.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "GameObject.h"
#include "OpenGL.h"
#include "Color.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleSceneManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleTimeManager.h"
#include "ModuleWindow.h"
#include "ModuleResourceManager.h"

#include "ModuleFileSystem.h"

#include "ResourceMesh.h"
#include "ResourceShader.h"
#include "ResourceTexture.h"
#include "ResourceMaterial.h"

#include "ImporterMeta.h"
#include "ImporterMaterial.h"
#include "ResourceMeta.h"

#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"

ComponentMeshRenderer::ComponentMeshRenderer(GameObject* ContainerGO): Component(ContainerGO, Component::ComponentType::MeshRenderer)
{
	material = (ResourceMaterial*)App->resources->GetResource(App->resources->GetDefaultMaterialUID());
}

ComponentMeshRenderer::~ComponentMeshRenderer()
{
	if (material && material->IsInMemory())
	{
		material->Release();
		material->RemoveUser(GO);
	}
}

void ComponentMeshRenderer::Draw(bool outline) const
{
	ComponentMesh * mesh = this->GO->GetComponent<ComponentMesh>();
	ComponentTransform* transform = GO->GetComponent<ComponentTransform>();
	ComponentCamera* camera = GO->GetComponent<ComponentCamera>();

	uint shader = App->renderer3D->defaultShader->ID;

	if(material)
	shader = material->shader->ID;

	float4x4 model = transform->GetGlobalTransform();

	if (outline)
	{
		shader = App->renderer3D->OutlineShader->ID;
		// --- Draw selected, pass scaled-up matrix to shader ---
		float3 scale = float3(1.05f, 1.05f, 1.05f);
		
		model = float4x4::FromTRS(model.TranslatePart(), model.RotatePart(), scale);
	}

	//mat->resource_material->UpdateUniforms();

	// --- Display Z buffer ---
	if (App->renderer3D->zdrawer)
	{
		shader = App->renderer3D->ZDrawerShader->ID;
	}

	glUseProgram(shader);

	// --- Set uniforms ---
	GLint modelLoc = glGetUniformLocation(shader, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.Transposed().ptr());
	
	GLint viewLoc = glGetUniformLocation(shader, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	GLint timeLoc = glGetUniformLocation(shader, "time");
	glUniform1f(timeLoc, App->time->time);

	float farp = App->renderer3D->active_camera->GetFarPlane();
	float nearp = App->renderer3D->active_camera->GetNearPlane();
	// --- Give ZDrawer near and far camera frustum planes pos ---
	if (App->renderer3D->zdrawer)
	{
		int nearfarLoc = glGetUniformLocation(shader, "nearfar");
		glUniform2f(nearfarLoc, nearp, farp);
	}

	// right handed projection matrix
	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV()*DEGTORAD / 2.0f);
	 float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, nearp, 0.0f);

	GLint projectLoc = glGetUniformLocation(shader, "projection");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());


	if (mesh && mesh->resource_mesh && mesh->IsEnabled())
	{
		//if (mesh->resource_def_mesh)
		//{
		//	DrawMesh(*mesh->resource_def_mesh);
		//}
		//else
		//{
			DrawMesh(*mesh->resource_mesh);
			DrawNormals(*mesh->resource_mesh, *transform);
		//}

	}

	glUseProgram(App->renderer3D->defaultShader->ID);

	// --- Draw Frustum ---
	if (camera)
		ModuleSceneManager::DrawWire(camera->frustum, White, App->scene_manager->GetPointLineVAO());

	if(App->scene_manager->display_boundingboxes)
	ModuleSceneManager::DrawWire(GO->GetAABB(), Green, App->scene_manager->GetPointLineVAO());
}

void ComponentMeshRenderer::DrawMesh(ResourceMesh& mesh) const
{
	if (mesh.vertices && mesh.Indices)
	{
		glBindVertexArray(mesh.VAO);

		if (this->checkers)
			glBindTexture(GL_TEXTURE_2D, App->textures->GetCheckerTextureID()); // start using texture
		else
		{
			if (material->resource_diffuse)
				glBindTexture(GL_TEXTURE_2D, material->resource_diffuse->GetTexID());
			else
				glBindTexture(GL_TEXTURE_2D, App->textures->GetDefaultTextureID());
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
		glDrawElements(GL_TRIANGLES, mesh.IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer (texture)
	}
}


void ComponentMeshRenderer::DrawNormals(const ResourceMesh& mesh, const ComponentTransform& transform) const
{
	// --- Draw Mesh Normals ---

	// --- Set Uniforms ---
	glUseProgram(App->renderer3D->linepointShader->ID);

	GLint modelLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform.GetGlobalTransform().Transposed().ptr());

	GLint viewLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	float nearp = App->renderer3D->active_camera->GetNearPlane();

	// right handed projection matrix
	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV()*DEGTORAD / 2.0f);
	float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, nearp, 0.0f);

	GLint projectLoc = glGetUniformLocation(App->renderer3D->linepointShader->ID, "projection");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());
	int vertexColorLocation = glGetAttribLocation(App->renderer3D->linepointShader->ID, "color");

	if (draw_vertexnormals && mesh.vertices->normal)
	{
		glVertexAttrib3f(vertexColorLocation, 1.0, 1.0, 0);

		// --- Draw Vertex Normals ---
		float3* vertices = new float3[mesh.IndicesSize * 2];

		for (uint i = 0; i < mesh.IndicesSize; ++i)
		{
			// --- Normals ---
			vertices[i * 2] = float3(mesh.vertices[mesh.Indices[i]].position[0], mesh.vertices[mesh.Indices[i]].position[1], mesh.vertices[mesh.Indices[i]].position[2]);
			vertices[(i * 2) + 1] = float3(mesh.vertices[mesh.Indices[i]].position[0] + mesh.vertices[mesh.Indices[i]].normal[0] * NORMAL_LENGTH, mesh.vertices[mesh.Indices[i]].position[1] + mesh.vertices[mesh.Indices[i]].normal[1] * NORMAL_LENGTH, mesh.vertices[mesh.Indices[i]].position[2] + mesh.vertices[mesh.Indices[i]].normal[2] * NORMAL_LENGTH);
		}

		// --- Create VAO, VBO ---
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(App->scene_manager->GetPointLineVAO());

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh.IndicesSize*2, vertices, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// --- Draw lines ---
		glLineWidth(3.0f);
		glBindVertexArray(App->scene_manager->GetPointLineVAO());
		glDrawArrays(GL_LINES, 0, mesh.IndicesSize * 2);
		glBindVertexArray(0);
		glLineWidth(1.0f);

		// --- Delete VBO and vertices ---
		glDeleteBuffers(1, &VBO);
		delete[] vertices;
	}
	
	// --- Draw Face Normals 

	if (draw_facenormals)
	{
		glVertexAttrib3f(vertexColorLocation, 0, 1.0, 1.0);
		Triangle face;
		float3* vertices = new float3[mesh.IndicesSize / 3 * 2];
		
		// --- Compute face normals ---
		for (uint j = 0; j < mesh.IndicesSize / 3; ++j)
		{
			face.a = float3(mesh.vertices[mesh.Indices[j * 3]].position);
			face.b = float3(mesh.vertices[mesh.Indices[(j * 3) + 1]].position);
			face.c = float3(mesh.vertices[mesh.Indices[(j * 3) + 2]].position);

			float3 face_center = face.Centroid();

			float3 face_normal = Cross(face.b - face.a, face.c - face.b);

			face_normal.Normalize();

			vertices[j * 2] = float3(face_center.x, face_center.y, face_center.z);
			vertices[(j * 2) + 1] = float3(face_center.x + face_normal.x * NORMAL_LENGTH, face_center.y + face_normal.y * NORMAL_LENGTH, face_center.z + face_normal.z * NORMAL_LENGTH);
		}

		// --- Create VAO, VBO ---
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(App->scene_manager->GetPointLineVAO());

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3 * mesh.IndicesSize / 3 * 2, vertices, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// --- Draw lines ---
		glLineWidth(3.0f);
		glColor3f(255, 255, 0);
		glBindVertexArray(App->scene_manager->GetPointLineVAO());
		glDrawArrays(GL_LINES, 0, mesh.IndicesSize / 3 * 2);
		glBindVertexArray(0);
		glColor3f(255, 255, 255);
		glLineWidth(1.0f);

		// --- Delete VBO and vertices ---
		glDeleteBuffers(1, &VBO);
		delete[] vertices;
	}

	glUseProgram(App->renderer3D->defaultShader->ID);
}

json ComponentMeshRenderer::Save() const
{
	json node;

	node["Resources"]["ResourceMaterial"] = std::string(material->GetResourceFile());

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

	return node;
}

void ComponentMeshRenderer::Load(json& node)
{
	std::string mat_path = node["Resources"]["ResourceMaterial"];

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();

	if(IMeta)
	{
		ResourceMeta* meta = (ResourceMeta*)IMeta->Load(mat_path.c_str());

		if (material)
			material->Release();

		if(meta)
		material = (ResourceMaterial*)App->resources->GetResource(meta->GetUID());

		// --- We want to be notified of any resource event ---
		if (material)
			material->AddUser(GO);
	}
}

void ComponentMeshRenderer::ONResourceEvent(uint UID, Resource::ResourceNotificationType type)
{
	// --- Always check if your resources are already invalidated, since go sends events from all of its components resources ---

	switch (type)
	{
	case Resource::ResourceNotificationType::Overwrite:
		if (material && UID == material->GetUID())
			material = (ResourceMaterial*)App->resources->GetResource(UID);
		break;

	case Resource::ResourceNotificationType::Deletion:
		if (material && UID == material->GetUID())
			material = nullptr;
		break;

	default:
		break;
	}
}

void ComponentMeshRenderer::CreateInspectorNode()
{
	ImGui::Checkbox("##RenActive", &GetActive());
	ImGui::SameLine();

	if (ImGui::TreeNode("Mesh Renderer"))
	{

		ImGui::Checkbox("Vertex Normals", &draw_vertexnormals);
		ImGui::SameLine();
		ImGui::Checkbox("Face Normals  ", &draw_facenormals);
		ImGui::SameLine();
		ImGui::Checkbox("Checkers", &checkers);

		ImGui::TreePop();
	}

	ImGui::NewLine();
	ImGui::Separator();
	ImGui::PushID("Material");

	// --- Material node ---
	if (material)
	{
		// --- Mat preview
		ImGui::Image((void*)(uint)material->GetPreviewTexID(), ImVec2(30, 30));
		ImGui::SameLine();
		
		if (ImGui::TreeNode(material->GetName()))
		{
			static ImGuiComboFlags flags = 0;

			ImGui::Text("Shader");
			ImGui::SameLine();

			const char* item_current = material->shader->name.c_str();
			if (ImGui::BeginCombo("##Shader", item_current, flags))
			{
				for (std::map<uint, ResourceShader*>::iterator it = App->resources->shaders.begin(); it != App->resources->shaders.end(); ++it)
				{
					bool is_selected = (item_current == it->second->name);

					if (ImGui::Selectable(it->second->name.c_str(), is_selected))
					{
						item_current = it->second->name.c_str();
						material->shader = it->second;
						material->shader->GetAllUniforms(material->uniforms);
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			// --- Print Texture Path ---
			//std::string Path = "Path: ";
			//Path.append(material->resource_diffuse->Texture_path);

			//ImGui::Text(Path.data());

			if (material->resource_diffuse)
			{
				// --- Print Texture Width and Height ---
				ImGui::Text(std::to_string(material->resource_diffuse->Texture_width).c_str());
				ImGui::SameLine();
				ImGui::Text(std::to_string(material->resource_diffuse->Texture_height).c_str());
			}

			//ImGui::Text("Shader Uniforms");
			//App->gui->panelShaderEditor->DisplayAndUpdateUniforms(material);
			//ImGui::TreePop();

			ImGui::NewLine();

			// --- Texture Preview ---
			if (material->resource_diffuse)
				ImGui::ImageButton((void*)(uint)material->resource_diffuse->GetPreviewTexID(), ImVec2(20, 20));
			else
				ImGui::ImageButton(NULL, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 2);
	
			// --- Handle drag & drop ---
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("resource"))
				{
					uint UID = *(const uint*)payload->Data;
					Resource* resource = App->resources->GetResource(UID, false);

					if (resource && resource->GetType() == Resource::ResourceType::TEXTURE)
					{
						if (material->resource_diffuse)
							material->resource_diffuse->Release();

						material->resource_diffuse = (ResourceTexture*)App->resources->GetResource(UID);

						// --- Save material so we update path to texture ---
						ImporterMaterial* IMat = App->resources->GetImporter<ImporterMaterial>();

						if(IMat)
						IMat->Save(material);
					}						
				}

				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			ImGui::Text("Albedo");
			ImGui::TreePop();
		}
	}

	ImGui::PopID();
}

