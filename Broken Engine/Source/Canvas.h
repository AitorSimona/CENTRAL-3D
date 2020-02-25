#pragma once
#include "Component.h"
#include "Color.h"
#include <vector>

class Canvas : public Component
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
	Canvas(GameObject* gameObject);
	~Canvas();

	void Draw() const;
	void CreateInspectorNode() override;

	void AddElement(Component* element) { elements.push_back(element); }

	// UI Functions
	State GetState() { return state; }

	void ChangeStateTo(State new_state) { state = new_state; }
	void ChangeColor(Color new_color) { color = new_color; }
	void Scale(float2 size) { size2D = size; }
	void Move(float2 pos) { position2D = pos; }
	void Rotate(float rot) { rotation2D = rot; }

	void UpdateCollider();
	void UpdateState();

	// Load & Save
	json Save() const;
	void Load(json& node);
	void ONResourceEvent(uint UID, Resource::ResourceNotificationType type);
	void CreateInspectorNode();

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

