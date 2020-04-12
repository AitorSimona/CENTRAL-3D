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
#include "ComponentAudioListener.h"

#include "ModuleSceneManager.h"
#include "ModuleRenderer3D.h"

#include "ModuleSelection.h"

#include "mmgr/mmgr.h"

using namespace Broken;

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled) {
	camera = new ComponentCamera(nullptr);
	name = "Camera3D";
}

ModuleCamera3D::~ModuleCamera3D() {
}

bool ModuleCamera3D::Init(json& file) {
	App->renderer3D->SetActiveCamera(camera);
	App->renderer3D->SetCullingCamera(camera);
	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start() {
	ENGINE_CONSOLE_LOG("Setting up the camera");

	bool ret = true;
	camera->frustum.SetPos(float3(0.0f, 25.0f, -50.0f));
	camera->SetFOV(60.0f);
	reference = camera->frustum.Pos();
	camera->Look({ 0.0f, 0.0f, 0.0f });
	FrameObject({ 0.0f, 0.0f, 0.0f });
	camera->SetCameraValues(m_CustomDefaultCameraValues);

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp() {
	ENGINE_CONSOLE_LOG("Cleaning camera");

	delete camera;

	return true;
}

void ModuleCamera3D::LoadStatus(const json& file) 
{
	//#ifdef BE_GAME_BUILD
	//if (file["Camera3D"].find("ActiveCamera") != file["Camera3D"].end()) {
	//	ResourceScene* scene = (ResourceScene*)App->resources->GetResource(file["SceneManager"]["MainScene"]);
	//	SetActiveScene(scene);
	//}
	//else {
	//	ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: Could not find main scene for game.", );
	//}
	//#endif
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt) 
{
	if (App->GetAppState() == AppState::EDITOR && App->gui->isSceneHovered) {
		m_CameraSpeedDeltaTime = m_CameraSpeed * dt;
		m_ScrollSpeedDeltaTime = m_ScrollSpeed * dt;
		m_FinalSpeed = m_CameraSpeedDeltaTime * m_SpeedMultiplicator;

		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			m_FinalSpeed *= 2.0f;
	}

	return UPDATE_CONTINUE;
}

void ModuleCamera3D::UpdateCamera()
{
	if (App->GetAppState() == AppState::EDITOR && App->gui->isSceneHovered) 
	{
		float3 newPos(0, 0, 0);

		// --- Move ---
		if (/*!App->gui->IsMouseCaptured() && */App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) {
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += camera->frustum.Front() * m_FinalSpeed;
			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= camera->frustum.Front() * m_FinalSpeed;

			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= camera->frustum.WorldRight() * m_FinalSpeed;
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += camera->frustum.WorldRight() * m_FinalSpeed;

			if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos -= camera->frustum.Up() * m_FinalSpeed * 0.7f;
			if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos += camera->frustum.Up() * m_FinalSpeed * 0.7f;

			if (abs(App->input->GetMouseWheel()) > 0)
				ModifySpeedMultiplicator();

			// --- Look Around ---
			CameraLookAround(m_CameraSpeedDeltaTime, camera->frustum.Pos());
		}
		// --- Zoom ---
		else if (/*!App->gui->IsMouseCaptured() && */abs(App->input->GetMouseWheel()) > 0)
			CameraZoom(m_ScrollSpeedDeltaTime);

		camera->frustum.SetPos(camera->frustum.Pos() + newPos);
		reference += newPos;

		// --- Camera Pan ---
		if (App->input->GetMouseButton(2) == KEY_REPEAT)
			CameraPan(m_ScrollSpeedDeltaTime);

		// --- Orbit Object ---
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
			CameraLookAround(m_CameraSpeedDeltaTime, reference);

		// --- Frame object ---

		// SELECTED TODO
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
			if (GameObject* GO = App->selection->GetLastSelected())
				FrameObject(GO);
			else
				FrameObject(float3(0.0f));
		}

		//App->scene_manager->CreateGrid(camera->frustum.Pos().Length()); // rip adaptive grid
	}

	// --- Mouse picking ---
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && App->gui->isSceneHovered)
	{
		float mouse_x = App->input->GetMouseX();
		float mouse_y = App->input->GetMouseY();

		OnMouseClick(mouse_x, mouse_y);
	}
}

void ModuleCamera3D::OnMouseClick(const float mouse_x, const float mouse_y) {
	// MYTODO: Make this easy to understand / explain
	// Scene window relative coords
	float normalized_x = (mouse_x - App->gui->sceneX) / App->gui->sceneWidth * (float)App->window->GetWindowWidth();
	float normalized_y = (mouse_y - App->gui->sceneY) / App->gui->sceneHeight * (float)App->window->GetWindowHeight();

	// mouse pos in range -1 - 1
	normalized_x = ((normalized_x / (float)App->window->GetWindowWidth()) - 0.5) * 2;
	normalized_y = ((1.0f - (normalized_y / (float)App->window->GetWindowHeight())) - 0.5) * 2;

	//ENGINE_CONSOLE_LOG("mouse_X: %f", normalized_x);
	//ENGINE_CONSOLE_LOG("mouse_Y: %f", normalized_y);

	LineSegment ray = App->renderer3D->active_camera->frustum.UnProjectLineSegment(normalized_x, normalized_y);
	
	ray.b = ray.a + (ray.Dir()*camera->GetFarPlane()*2);

	last_ray = ray;

	App->selection->SceneRectangleSelect({ ray.a });

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE)
		App->scene_manager->SelectFromRay(ray);
}

void ModuleCamera3D::FrameObject(GameObject* GO) {
	if (GO) {
		ComponentTransform* transform = GO->GetComponent<ComponentTransform>();
		ComponentMesh* mesh = GO->GetComponent<ComponentMesh>();

		if (transform)
			FrameObject(transform->GetGlobalPosition());

		if (mesh && mesh->resource_mesh)
		{
			float3 center = GO->GetAABB().CenterPoint();

			reference.x = center.x;
			reference.y = center.y;
			reference.z = center.z;

			float3 Movement = camera->frustum.Front() * (2 * GO->GetAABB().Diagonal().Length());

			if (Movement.IsFinite())
				camera->frustum.SetPos(reference - Movement);
		}
	}
}

void ModuleCamera3D::FrameObject(float3 posToLook) {
	reference = posToLook;
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

	if (dx != 0) {
		camera->frustum.SetPos(camera->frustum.Pos() + (speed * camera->frustum.WorldRight() * dx * factor));
		reference += speed * camera->frustum.WorldRight() * dx * factor;
	}

	if (dy != 0) {
		camera->frustum.SetPos(camera->frustum.Pos() + speed * camera->frustum.Up() * dy * factor);
		reference += speed * camera->frustum.Up() * dy * factor;
	}
}

void ModuleCamera3D::CameraZoom(float speed) {
	float factor = abs(camera->frustum.Pos().y);
	if (factor < 1.0f)
		factor = 1.0f;

	int mouse_wheel = App->input->GetMouseWheel();
	float3 Movement = camera->frustum.Front() * mouse_wheel * speed * factor;
	camera->frustum.SetPos(camera->frustum.Pos() + Movement);
}

void ModuleCamera3D::ModifySpeedMultiplicator()
{
	m_ScrollingSpeedChange = true;
	m_SpeedMultiplicator += (m_ScrollSpeedDeltaTime * App->input->GetMouseWheel() * 5.0f);

	if (m_SpeedMultiplicator > 15.0f)
		m_SpeedMultiplicator = 15.0f;
	if (m_SpeedMultiplicator < 0.3f)
		m_SpeedMultiplicator = 0.3f;
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

	if (!reference.Equals(this->reference))
		this->reference = camera->frustum.Pos() + camera->frustum.Front() * (camera->frustum.Pos()).Length();
}