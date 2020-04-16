#ifndef __COMPONENTCANVAS_H__
#define __COMPONENTCANVAS_H__
#pragma once

#include "Component.h"
#include "Color.h"
#include "Math.h"
#include <vector>

BE_BEGIN_NAMESPACE

class BROKEN_API ComponentCanvas : public Component
{
public:
	ComponentCanvas(GameObject* gameObject);
	virtual ~ComponentCanvas();

	void Update() override;

	void Draw() const;
	void AddElement(Component* element) { elements.push_back(element); }
	static inline Component::ComponentType GetType() { return Component::ComponentType::Canvas; }

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

public:
	bool visible = true;
	int priority = 0;

private:
	std::vector<Component*> elements;
};

BE_END_NAMESPACE

#endif

