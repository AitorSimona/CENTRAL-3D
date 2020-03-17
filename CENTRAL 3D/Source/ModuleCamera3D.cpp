#include "Globals.h"
#include "Imgui/imgui.h"
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

#include "PanelScene.h"

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
	camera->frustum.SetPos(float3(0.0f,25.0f,-50.0f));
	camera->SetFOV(60.0f);
	reference = camera->frustum.Pos();
	camera->Look({ 0.0f, 0.0f, 0.0f });

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
	if (App->GetAppState() == AppState::EDITOR && App->gui->panelScene->SceneHovered)
	{
		speed = 10.0f * dt;
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speed *= 2.0f;
	}

	return UPDATE_CONTINUE;
}

void ModuleCamera3D::UpdateCamera()
{
	if (App->GetAppState() == AppState::EDITOR && App->gui->panelScene->SceneHovered)
	{
		float3 newPos(0, 0, 0);

		// --- Move ---
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
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
		if (abs(App->input->GetMouseWheel()) > 0)
			CameraZoom(speed);

		// --- Orbit Object ---
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
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

		App->scene_manager->CreateGrid(camera->frustum.Pos().Length());
	}
}

void ModuleCamera3D::OnMouseClick(const float mouse_x, const float mouse_y)
{
	// MYTODO: Make this easy to understand / explain

	// Scene window relative coords
	float normalized_x = (mouse_x - App->gui->panelScene->posX) / App->gui->panelScene->width * (float)App->window->GetWindowWidth();
	float normalized_y = (mouse_y - App->gui->panelScene->posY) / App->gui->panelScene->height * (float)App->window->GetWindowHeight();

	// mouse pos in range -1 - 1
	normalized_x = ((normalized_x / (float)App->window->GetWindowWidth()) - 0.5) * 2;
	normalized_y = ((1.0f - (normalized_y / (float)App->window->GetWindowHeight())) - 0.5) * 2;

	//CONSOLE_LOG("mouse_X: %f", normalized_x);
	//CONSOLE_LOG("mouse_Y: %f", normalized_y);

	LineSegment ray = App->renderer3D->active_camera->frustum.UnProjectLineSegment(normalized_x, normalized_y);

	ray.b = ray.a + ray.Dir() * camera->frustum.Pos().Length();

	last_ray = ray;


	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE)
		App->scene_manager->SelectFromRay(ray);
}

void ModuleCamera3D::FrameObject(GameObject* GO)
{
	if (GO)
	{
		ComponentTransform* transform = GO->GetComponent<ComponentTransform>();
		ComponentMesh* mesh = GO->GetComponent<ComponentMesh>();

		if (transform)
			Frame(transform->GetGlobalPosition());

		if (mesh && mesh->resource_mesh)
		{
			float3 center = GO->GetAABB().CenterPoint();

			reference.x = center.x;
			reference.y = center.y;
			reference.z = center.z;

			float3 Movement = camera->frustum.Front() * (2 * mesh->GetAABB().HalfDiagonal().Length());

			if(Movement.IsFinite())
				camera->frustum.SetPos(reference - Movement);
		}
		
	}
}

void ModuleCamera3D::Frame(float3 position)
{
	reference = position;
	float3 Movement = camera->frustum.Front() * 30.0f;

	if (Movement.IsFinite())
		camera->frustum.SetPos(reference - Movement);
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
