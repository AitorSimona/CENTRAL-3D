#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"
#include "ModuleInput.h"

#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

#include "ResourceMesh.h"
#include "ComponentCamera.h"

#include "ModuleSceneManager.h"

#include "mmgr/mmgr.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	camera = new ComponentCamera(nullptr);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;
	camera->frustum.pos = { 0.0f,1.0f,-5.0f };

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	delete camera;

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	float3 newPos(0,0,0);
	float speed = 10.0f * dt;
	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 20.0f * dt;

	// --- Move ---
	if (!App->gui->IsMouseCaptured() && App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += camera->frustum.front * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= camera->frustum.front * speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos += math::Cross(camera->frustum.up, camera->frustum.front) * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos -= math::Cross(camera->frustum.up, camera->frustum.front) * speed;

		// --- Look Around ---
		CameraLookAround();
	}

	camera->frustum.pos += newPos;
	reference += newPos;

	// --- Camera Pan ---
	if (App->input->GetMouseButton(2) == KEY_REPEAT)
		CameraPan(speed);

	// --- Zoom ---
	if (!App->gui->IsMouseCaptured() && abs(App->input->GetMouseWheel()) > 0)
		CameraZoom(speed);

	// --- Orbit Object ---
	if (!App->gui->IsMouseCaptured() && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
		CameraOrbit();

	// --- Frame object ---
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		FrameObject(App->scene_manager->GetSelectedGameObject());


	// Recalculate matrix -------------
	//CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference)
{
	//this->camera->frustum.pos = Position;
	//this->reference = Reference;

	//this->camera->frustum.front = (Position - Reference).Normalized;
	//X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	//Y = cross(Z, X);

	//if(!RotateAroundReference)
	//{
	//	this->reference = this->camera->frustum.pos;
	//	this->camera->frustum.pos += Z * 0.05f;
	//}

	//CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const float3 &Spot)
{
	reference = Spot;

	//Z = normalize(Position - Reference);
	//X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	//Y = cross(Z, X);

	math::float3 Z = -(camera->frustum.pos - reference).Normalized(); // Direction the camera is looking at (reverse direction of what the camera is targeting)
	math::float3 X = math::Cross(math::float3(0.0f, 1.0f, 0.0f), Z).Normalized(); // X is perpendicular to vectors Y and Z
	math::float3 Y = math::Cross(Z, X); // Y is perpendicular to vectors Z and X

	camera->frustum.front = Z;
	camera->frustum.up = Y;

	//CalculateViewMatrix();
}


void ModuleCamera3D::FrameObject(GameObject* GO)
{
	// MYTODO: Try to clean this...
	if (GO)
	{
		ComponentTransform* transform = GO->GetComponent<ComponentTransform>(Component::ComponentType::Transform);

		if (transform)
		{
			reference.x = transform->GetPosition().x;
			reference.y = transform->GetPosition().y;
			reference.z = transform->GetPosition().z;

			ComponentMesh* mesh = GO->GetComponent<ComponentMesh>(Component::ComponentType::Mesh);

			if (mesh)
			{

				Sphere s(transform->GetPosition(), 1);
				s.Enclose(mesh->resource_mesh->Vertices, mesh->resource_mesh->VerticesSize);

				s.r = s.Diameter() - Length(float3(reference.x, reference.y, reference.z));
				s.pos = transform->GetPosition();

				Look(camera->frustum.pos, float3(s.Centroid().x, s.Centroid().y, s.Centroid().z), true);
				float3 Movement = -camera->frustum.front * (2 * s.r);
				camera->frustum.pos = reference - Movement;

			}
		}
	}
}

void ModuleCamera3D::CameraPan(float speed)
{
	int dx = -App->input->GetMouseXMotion();
	int dy = App->input->GetMouseYMotion();
	float factor = abs(camera->frustum.pos.y) / 100.0f;
	if (factor < 0.5f)
		factor = 0.5f;

	if (dx != 0)
	{
		camera->frustum.pos -= speed * math::Cross(camera->frustum.up, camera->frustum.front) * dx*factor;
		reference -= speed * math::Cross(camera->frustum.up, camera->frustum.front) * dx*factor;
	}

	if (dy != 0)
	{
		camera->frustum.pos += speed * camera->frustum.up * dy*factor;
		reference += speed * camera->frustum.up * dy*factor;
	}
}

void ModuleCamera3D::CameraZoom(float speed)
{
	float factor = abs(camera->frustum.pos.y);
	if (factor < 1.0f)
		factor = 1.0f;

	int mouse_wheel = App->input->GetMouseWheel();
	float3 Movement = camera->frustum.front * mouse_wheel*speed*factor;
	camera->frustum.pos += Movement;
}

void ModuleCamera3D::CameraOrbit()
{
	/*int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	float Sensitivity = 0.25f;

	camera->frustum.pos -= reference;

	if (dx != 0)
	{
		float DeltaX = (float)dx * Sensitivity;

		X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));

		camera->frustum.up = float3::
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * Sensitivity;

		Y = rotate(Y, DeltaY, X);
		Z = rotate(Z, DeltaY, X);

		if (Y.y < 0.0f)
		{
			Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = cross(Z, X);
		}
	}

	Position = Reference + Z * length(Position);*/
}

void ModuleCamera3D::CameraLookAround()
{

	//int dx = -App->input->GetMouseXMotion();
	//int dy = -App->input->GetMouseYMotion();

	//float Sensitivity = 0.25f;

	//reference = camera->frustum.pos;
	//camera->frustum.pos -= reference;

	//if (dx != 0)
	//{
	//	float DeltaX = (float)dx * Sensitivity;

	//	//X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
	//	camera->frustum.up = rotate(camera->frustum.up, DeltaX, float3(0.0f, 1.0f, 0.0f));
	//	camera->frustum.front = rotate(camera->frustum.front, DeltaX, float3(0.0f, 1.0f, 0.0f));
	//}

	//if (dy != 0)
	//{
	//	float DeltaY = (float)dy * Sensitivity;

	//	camera->frustum.up = (camera->frustum.up, DeltaY, math::Cross(camera->frustum.up, camera->frustum.front));
	//	camera->frustum.front = rotate(camera->frustum.front, DeltaY, math::Cross(camera->frustum.up, camera->frustum.front));

	//	if (camera->frustum.up.y < 0.0f)
	//	{
	//		camera->frustum.front = float3(0.0f, camera->frustum.front.y > 0.0f ? 1.0f : -1.0f, 0.0f);
	//		camera->frustum.up = math::Cross(camera->frustum.front, math::Cross(camera->frustum.up, camera->frustum.front));
	//	}
	//}

	//camera->frustum.pos = reference + camera->frustum.front * (camera->frustum.pos).Length();
	//reference = camera->frustum.pos - camera->frustum.front * (camera->frustum.pos).Length();
}
