#include "Application.h"
#include "ComponentBone.h"
#include "ComponentTransform.h"
#include "ComponentAnimation.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleCamera3D.h"
#include "ComponentCamera.h"
#include "ModuleRenderer3D.h"
#include "ResourceShader.h"
#include "ModuleSceneManager.h"

#include "GameObject.h"
#include "Imgui/imgui.h"
#include "OpenGL.h"


ComponentBone::ComponentBone(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::Bone)
{

}

ComponentBone::~ComponentBone()
{
	if (res_bone && res_bone->IsInMemory())
	{
		res_bone->Release();
		res_bone->RemoveUser(GO);
	}
}

void ComponentBone::DebugDrawBones()
{

	if (GO->parent != nullptr && GO->parent->GetComponent<ComponentBone>() != nullptr
		&& GO->GetAnimGO(GetHipBone()->GO)->GetComponent<ComponentAnimation>()->draw_bones)
	{
		float farp = App->renderer3D->active_camera->GetFarPlane();
		float nearp = App->renderer3D->active_camera->GetNearPlane();
		// right handed projection matrix
		float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
		float4x4 proj_RH(
			f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
			0.0f, f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, nearp, 0.0f);

		//Set polygon draw mode and appropiated matrices for OGL
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glLoadIdentity();
		
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(proj_RH.ptr());
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

		glPushMatrix();
		glMultMatrixf(GetBoneTransform().Transposed().ptr());

		//uint shader = App->renderer3D->linepointShader->ID;
		//glUseProgram(shader);
		//// --- Set uniforms ---

		//GLint modelLoc = glGetUniformLocation(shader, "model_matrix");
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, GetBoneTransform().Transposed().ptr());

		//GLint viewLoc = glGetUniformLocation(shader, "view");
		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

		//GLint projectLoc = glGetUniformLocation(shader, "projection");
		//glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());


		//float3* vertices = new float3[2];
		/*float4x4 child_matrix = GetBoneTransform();
		float4x4 parent_matrix = GO->parent->GetComponent<ComponentBone>()->GetBoneTransform();*/
		float3 child_pos = GO->GetComponent<ComponentTransform>()->GetGlobalPosition();
		float3 parent_pos = GO->parent->GetComponent<ComponentTransform>()->GetGlobalPosition();
		/*vertices[0] = child_pos;
		vertices[1] = parent_pos;*/

		//// --- Create VAO, VBO ---
		//unsigned int VBO;
		//glGenBuffers(1, &VBO);
		//// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		//glBindVertexArray(App->scene_manager->GetPointLineVAO());

		//glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*2, vertices, GL_DYNAMIC_DRAW);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		//glEnableVertexAttribArray(0);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);

		//// --- Draw lines ---
		//glLineWidth(3.0f);
		//glBindVertexArray(App->scene_manager->GetPointLineVAO());
		//glDrawArrays(GL_LINES, 0, 2);
		//glBindVertexArray(0);
		//glLineWidth(1.0f);

		// --- Delete VBO and vertices ---
		/*glDeleteBuffers(1, &VBO);*/
		//delete[] vertices;

		//glUseProgram(App->renderer3D->defaultShader->ID);

		



		glLineWidth(5.0f);
		glBegin(GL_LINES);

		//Yellow
		glColor4f(1.0, 1.0, 0.0, 1.0);

		glVertex3f(child_pos.x, child_pos.y, child_pos.z);
		glVertex3f(parent_pos.x, parent_pos.y, parent_pos.z);

		glEnd();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glLineWidth(1.0f);


		//Set again Identity for OGL Matrices & Polygon draw to fill again
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
}

float4x4 ComponentBone::GetBoneTransform() const
{
	return GO->GetComponent<ComponentTransform>()->GetGlobalTransform();
}

float4x4 ComponentBone::GetSkeletonTransform()
{
	return GO->GetAnimGO(GetHipBone()->GO)->GetComponent<ComponentTransform>()->GetGlobalTransform().Inverted() * GO->GetComponent<ComponentTransform>()->GetGlobalTransform();
}


ComponentBone* ComponentBone::GetHipBone()
{
	ComponentBone* pBone = GO->parent->GetComponent<ComponentBone>();

	if (pBone == nullptr)
	{
		return this;
	}
	else
		return pBone->GetHipBone();
}

json ComponentBone::Save() const
{
	json node;

	// --- Store path to component file ---
	if (res_bone)
		node["Resources"]["ResourceBone"] = std::string(res_bone->GetResourceFile());


	return node;
}

void ComponentBone::Load(json& node)
{
	std::string path = node["Resources"]["ResourceBone"];
	App->fs->SplitFilePath(path.c_str(), nullptr, &path);
	path = path.substr(0, path.find_last_of("."));

	if (res_bone)
		res_bone->Release();

	res_bone = (ResourceBone*)App->resources->GetResource(std::stoi(path));

	// --- We want to be notified of any resource event ---
	if (res_bone)
		res_bone->AddUser(GO);
}

void ComponentBone::ONResourceEvent(uint UID, Resource::ResourceNotificationType type)
{
	// --- Always check if your resources are already invalidated, since go sends events from all of its components resources ---
	switch (type)
	{
	case Resource::ResourceNotificationType::Overwrite:
		if (res_bone && UID == res_bone->GetUID())
			res_bone = (ResourceBone*)App->resources->GetResource(UID);
		break;

	case Resource::ResourceNotificationType::Deletion:
		if (res_bone && UID == res_bone->GetUID())
			res_bone = nullptr;
		break;

	default:
		break;
	}
}

void ComponentBone::CreateInspectorNode()
{
	ImGui::Checkbox("##Bone", &GetActive());
	ImGui::SameLine();

	if (ImGui::TreeNode("Bone"))
	{
		ImGui::Text("Im a component bone :D");
		
		if (res_bone)
		{
			ImGui::Text("ResourceBone linked");
			ImGui::Text("Name: %s", res_bone->GetName());
		}
			


		ImGui::TreePop();
	}
}
