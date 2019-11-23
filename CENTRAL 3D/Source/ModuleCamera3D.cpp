#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"

#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

#include "ResourceMesh.h"
#include "ComponentCamera.h"

#include "ModuleSceneManager.h"
#include "ModuleRenderer3D.h"

#include "mmgr/mmgr.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	camera = new ComponentCamera(nullptr);
}

ModuleCamera3D::~ModuleCamera3D()
{}

bool ModuleCamera3D::Init(json config)
{
	App->renderer3D->SetActiveCamera(camera);
	App->renderer3D->SetCullingCamera(camera);


	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	CONSOLE_LOG("Setting up the camera");
	bool ret = true;
	camera->frustum.SetPos(float3(0.0f,1.0f,-5.0f));
	reference = camera->frustum.Pos();
	camera->update_projection = true;


	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	CONSOLE_LOG("Cleaning camera");

	delete camera;

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	if (App->GetAppState() == AppState::EDITOR)
	{
		float3 newPos(0, 0, 0);
		float speed = 10.0f * dt;
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speed *= 2.0f;

		// --- Move ---
		if (!App->gui->IsMouseCaptured() && App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += camera->frustum.Front() * speed;
			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= camera->frustum.Front() * speed;


			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= camera->frustum.WorldRight() * speed;
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += camera->frustum.WorldRight() * speed;


			// --- Look Around ---
			CameraLookAround(speed, camera->frustum.Pos());
		}

		camera->frustum.SetPos(camera->frustum.Pos() + newPos);
		reference += newPos;

		// --- Camera Pan ---
		if (App->input->GetMouseButton(2) == KEY_REPEAT)
			CameraPan(speed);

		// --- Zoom ---
		if (!App->gui->IsMouseCaptured() && abs(App->input->GetMouseWheel()) > 0)
			CameraZoom(speed);

		// --- Orbit Object ---
		if (!App->gui->IsMouseCaptured() && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
			CameraLookAround(speed, reference);

		// --- Frame object ---
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
			FrameObject(App->scene_manager->GetSelectedGameObject());


		// --- Mouse picking ---
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			float mouse_x = App->input->GetMouseX();
			float mouse_y = App->input->GetMouseY();

			OnMouseClick(mouse_x, mouse_y);
		}
	}

	return UPDATE_CONTINUE;
}

void ModuleCamera3D::OnMouseClick(const float mouse_x, const float mouse_y)
{
	float normalized_x = mouse_x / (float)App->window->GetWindowWidth();
	float normalized_y = 1.0f - (mouse_y / (float)App->window->GetWindowHeight());

	// --- Normalizing mouse position --- 
	normalized_x = (normalized_x - 0.5) / 0.5;
	normalized_y = (normalized_y - 0.5) / 0.5;

	LineSegment ray = App->renderer3D->active_camera->frustum.UnProjectLineSegment(normalized_x, normalized_y);
	last_ray = ray;

	App->scene_manager->SelectFromRay(ray);
}

void ModuleCamera3D::FrameObject(GameObject* GO)
{
	if (GO)
	{
		ComponentTransform* transform = GO->GetComponent<ComponentTransform>(Component::ComponentType::Transform);
		ComponentMesh* mesh = GO->GetComponent<ComponentMesh>(Component::ComponentType::Mesh);

		if (mesh && mesh->resource_mesh)
		{
			AABB aabb;
			aabb.SetNegativeInfinity();
			aabb.Enclose(mesh->resource_mesh->Vertices, mesh->resource_mesh->VerticesSize);
			aabb.Translate(transform->GetPosition());
			float3 center = aabb.CenterPoint();

			reference.x = center.x;
			reference.y = center.y;
			reference.z = center.z;

			float3 Movement = camera->frustum.Front() * (2 * mesh->GetAABB().HalfDiagonal().Length());
			camera->frustum.SetPos(reference - Movement);
		}
		
	}
}


void ModuleCamera3D::CameraPan(float speed)
{
	int dx = -App->input->GetMouseXMotion();
	int dy = App->input->GetMouseYMotion();
	float factor = abs(camera->frustum.Pos().y) / 100.0f;
	if (factor < 0.5f)
		factor = 0.5f;

	if (dx != 0)
	{
		camera->frustum.SetPos(camera->frustum.Pos() + (speed * camera->frustum.WorldRight() * dx*factor));
		reference += speed * camera->frustum.WorldRight() * dx*factor;
	}

	if (dy != 0)
	{
		camera->frustum.SetPos(camera->frustum.Pos() + speed * camera->frustum.Up() * dy*factor);
		reference += speed * camera->frustum.Up() * dy*factor;
	}
}

void ModuleCamera3D::CameraZoom(float speed)
{
	float factor = abs(camera->frustum.Pos().y);
	if (factor < 1.0f)
		factor = 1.0f;

	int mouse_wheel = App->input->GetMouseWheel();
	float3 Movement = camera->frustum.Front() * mouse_wheel*speed*factor;
	camera->frustum.SetPos(camera->frustum.Pos() + Movement);
}

void ModuleCamera3D::CameraLookAround(float speed, float3 reference)
{
	float dx = -App->input->GetMouseXMotion()*speed;
	float dy = -App->input->GetMouseYMotion()*speed;

	math::Quat rotationX = math::Quat::RotateAxisAngle(float3::unitY, dx * DEGTORAD);
	math::Quat rotationY = math::Quat::RotateAxisAngle(camera->frustum.WorldRight(), dy * DEGTORAD);
	math::Quat finalRotation = rotationX * rotationY;

	camera->frustum.SetUp(finalRotation.Mul(camera->frustum.Up()).Normalized());
	camera->frustum.SetFront(finalRotation.Mul(camera->frustum.Front()).Normalized());

	float distance = (camera->frustum.Pos() - reference).Length();
	camera->frustum.SetPos(reference + (-camera->frustum.Front() * distance));

	if(!reference.Equals(this->reference))
	this->reference = camera->frustum.Pos() + camera->frustum.Front() * (camera->frustum.Pos()).Length();
}
