#ifndef __CANVAS_H__
#define __CANVAS_H__
#pragma once

#include "Component.h"
#include "Color.h"
#include "Math.h"
#include <vector>

BE_BEGIN_NAMESPACE

class ResourceTexture;

class BROKEN_API ComponentCanvas : public Component
{
public:
	ComponentCanvas(GameObject* gameObject);
	virtual ~ComponentCanvas();

	void Update() override;

	void Draw() const;
	void AddElement(Component* element) { elements.push_back(element); }

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

public:
	bool visible = true;

	// for debug purposes
	float2 size2D = { 0,0 };
	float2 position2D = { 0,0 };

private:
	ResourceTexture* texture = nullptr;
	std::vector<Component*> elements;
};

BE_END_NAMESPACE

#endif

