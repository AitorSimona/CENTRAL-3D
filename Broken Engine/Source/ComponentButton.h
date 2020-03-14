#pragma once
#include "Component.h"
#include "ComponentCanvas.h"

BE_BEGIN_NAMESPACE

class ResourceTexture;

class BROKEN_API ComponentButton : public Component
{
public:
	enum State
	{
		NOTHING = 0,
		IDLE,
		HOVERED,
		SELECTED,
		LOCKED,
		DRAGGING
	};

	ComponentButton(GameObject* gameObject);
	virtual ~ComponentButton();

	void Draw();

	State GetState() { return state; }
	void UpdateState();
	void ChangeStateTo(State new_state) { state = new_state; }
	void ChangeColorTo(Color new_color) { color = new_color; }
	void UpdateCollider();
	void OnClick();


	// UI Functions
	void Scale(float2 size) { size2D = size; }
	void Move(float2 pos) { position2D = pos; }
	void Rotate(float rot) { rotation2D = rot; }

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

public:
	bool visible = true;
	bool interactable = true;
	bool draggable = false;

	float2 size2D = { 1,1 };
	float2 position2D = { 0,0 };
	float rotation2D = 0.0f;

	State state = NOTHING;

public:
	ComponentCanvas* canvas = nullptr;
	ResourceTexture* texture = nullptr;

private:
	SDL_Rect collider;
	Color color;

	Color idle_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Color hovered_color = { 0.5f, 0.5f, 0.5f, 1.0f };
	Color selected_color = { 0.25f, 0.25f, 1.0f, 1.0f };
	Color locked_color = { 1.0f, 1.0f, 1.0f, 1.0f };
};

BE_END_NAMESPACE
