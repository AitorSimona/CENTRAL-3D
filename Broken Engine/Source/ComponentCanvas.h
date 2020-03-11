#ifndef __CANVAS_H__
#define __CANVAS_H__
#pragma once

#include "Component.h"
#include "Color.h"
#include "Math.h"
#include <vector>

BE_BEGIN_NAMESPACE

class BROKEN_API ComponentCanvas : public Component
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

public:
	ComponentCanvas(GameObject* gameObject);
	virtual ~ComponentCanvas();

	void Update() override;

	void Draw() const;
	void AddElement(Component* element) { elements.push_back(element); }

	// UI Functions
	State GetState() { return state; }

	void ChangeStateTo(State new_state) { state = new_state; }
	void ChangeColor(Color new_color) { color = new_color; }
	void Scale(float2 size) { size2D = size; }
	void Move(float2 pos) { position2D = pos; }
	void Rotate(float rot) { rotation2D = rot; }

	void UpdatePosition();
	void UpdateCollider();
	void UpdateState();

	// Elements
	void MoveElement(float2 new_pos, Component* element);
	void RotateElement(float new_angle, Component* element);
	void ScaleElement(float2 new_scale, Component* element);	

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

public:
	bool visible = true;
	bool draggable = true;
	bool interactable = true;

	float2 size2D = { 50,50 };
	float2 position2D = { 50,50 };
	float rotation2D = 0.0f;

	Color color = White;

protected:
	State state = NOTHING;
	SDL_Rect collider = { 0,0,0,0 };

private:
	//ResourceTexture* texture = nullptr;
	std::vector<Component*> elements;
};

BE_END_NAMESPACE

#endif

