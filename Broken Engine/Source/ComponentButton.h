#pragma once
#include "Component.h"
#include "ComponentCanvas.h"
#include "ComponentImage.h"
#include "ComponentText.h"

class ComponentButton : public Component
{
	enum State
	{
		NOTHING = 0,
		IDLE,
		HOVERED,
		SELECTED,
		LOCKED,
		DRAGGING
	};

public:
	ComponentButton(GameObject* gameObject);
	virtual ~ComponentButton();

	void Draw();

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

public:
	ComponentCanvas* canvas = nullptr;
	ResourceTexture* img_texture = nullptr;
	ResourceTexture* txt_texture = nullptr;

private:
	Color idle_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Color hovered_color = { 0.5f, 0.5f, 0.5f, 1.0f };
	Color selected_color = { 0.25f, 0.25f, 1.0f, 1.0f };
	Color locked_color = { 1.0f, 1.0f, 1.0f, 1.0f };
};
