#pragma once
#include "Component.h"
#include "ComponentCanvas.h"
#include "ComponentScript.h"

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

	void Update() override;

	void Draw();

	State GetState() { return state; }
	void UpdateState();
	void OnClick();
	void ChangeStateTo(State new_state) { state = new_state; }
	void ChangeColorTo(Color new_color) { color = new_color; }

	// UI Functions
	void Scale(float2 size) { size2D = size; }
	void Move(float2 pos) { position2D = pos; }
	void Rotate(float rot) { rotation2D = rot; }
	static inline Component::ComponentType GetType() { return Component::ComponentType::Button; }

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;
	void SetNullptr();

public:
	bool visible = true;
	bool interactable = true;
	bool draggable = false;
	bool resize = true;

	float2 size2D = { 1,1 };
	float2 position2D = { 0,0 };
	float rotation2D = 0.0f;

	State state = NOTHING;

public:
	ComponentCanvas* canvas = nullptr;
	ResourceTexture* texture = nullptr;
	ComponentScript* script = nullptr;
	GameObject* script_obj = nullptr;

private:
	SDL_Rect collider;
	bool collider_visible = true;

	std::string func_name;
	std::vector<const char*> func_list;
	uint func_pos = 0;

	Color color;
	Color idle_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Color hovered_color = { 0.5f, 0.5f, 0.5f, 1.0f };
	Color selected_color = { 0.25f, 0.25f, 1.0f, 1.0f };
	Color locked_color = { 1.0f, 1.0f, 1.0f, 1.0f };
};

BE_END_NAMESPACE
