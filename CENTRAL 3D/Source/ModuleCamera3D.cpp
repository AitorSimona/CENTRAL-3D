#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"
#include "ModuleInput.h"

#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

#include "ModuleSceneManager.h"

#include "mmgr/mmgr.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);

}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	Move(vec3(1.0f, 1.0f, 0.0f));
	LookAt(vec3(0, 0, 0));

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");


	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	vec3 newPos(0,0,0);
	float speed = 10.0f * dt;
	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 20.0f * dt;

	if (!App->gui->IsMouseCaptured() && App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;
	}

	Position += newPos;
	Reference += newPos;

	// --- Camera Pan ---
	if (App->input->GetMouseButton(2) == KEY_REPEAT)
	{
		int dx = App->input->GetMouseXMotion();
		int dy = App->input->GetMouseYMotion();
		float factor = abs(Position.y)/100.0f;
		if (factor < 0.5f)
			factor = 0.5f;

		if (dx != 0)
		{
			Position -= speed*X * dx*factor;
			Reference -= speed*X * dx*factor;
		}

		if (dy != 0)
		{
			Position += speed *Y * dy*factor;
			Reference += speed *Y * dy*factor;
		}
	}

	// --- Zoom ---
	if (!App->gui->IsMouseCaptured() && abs(App->input->GetMouseWheel()) > 0)
	{
		float factor = abs(Position.y);
		if (factor < 1.0f)
			factor = 1.0f;

		int mouse_wheel = App->input->GetMouseWheel();
		vec3 Movement = -Z*mouse_wheel*speed*factor;
		Position += Movement;
	}

	// --- Orbit Object ---
	if (!App->gui->IsMouseCaptured() && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		Position -= Reference;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
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

		Position = Reference + Z * length(Position);
	}

	// --- Frame object ---
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		FrameObject(*App->scene_manager->GetGameObjects().at(App->scene_manager->GetSelectedGameObjects()));
	}

	// Mouse motion ----------------

	if(!App->gui->IsMouseCaptured() && App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		//vec3 Reference_backup = Reference;
		Reference = Position;
		Position -= Reference;

		if(dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if(dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Y = rotate(Y, DeltaY, X);
			Z = rotate(Z, DeltaY, X);

			if(Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
			}
		}

		Position = Reference + Z * length(Position);
		Reference = Position - Z * length(Position);
	}



	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

void ModuleCamera3D::FrameObject(GameObject& GO)
{
	ComponentTransform* transform = GO.GetComponent<ComponentTransform>(Component::ComponentType::Transform);

	Reference.x = transform->GetPosition().x;
	Reference.y = transform->GetPosition().y;
	Reference.z = transform->GetPosition().z;

	ComponentMesh* mesh = GO.GetComponent<ComponentMesh>(Component::ComponentType::Mesh);

	Sphere s(transform->GetPosition(), 1);
	s.Enclose(mesh->Vertices, mesh->VerticesSize);

	s.r = s.Diameter() - Length(float3(Reference.x, Reference.y, Reference.z));
	s.pos = transform->GetPosition();

	Look(Position, vec3(s.Centroid().x, s.Centroid().y, s.Centroid().z), true);
	vec3 Movement = -Z * (s.r);
	Position = Reference - Movement;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}