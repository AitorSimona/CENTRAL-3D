#ifndef __COMPONENTIMAGE_H__
#define __COMPONENTIMAGE_H__
#pragma once

#include "Component.h"
#include "Math.h"

BE_BEGIN_NAMESPACE

class ComponentCanvas;
class ResourceTexture;

class BROKEN_API ComponentImage : public Component
{
public:
	ComponentImage(GameObject* gameObject);
	virtual ~ComponentImage();

	void Update() override;

	void Draw();

	// UI Functions
	void Scale(float2 size) { size2D = size; }
	void Move(float2 pos) { position2D = pos; }
	void Rotate(float rot) { rotation2D = rot; }

	static inline Component::ComponentType GetType() { return Component::ComponentType::Image; }

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

public:
	bool visible = true;
	bool resize = true;

	float2 size2D = { 50, 50 };
	float2 position2D = { 0,0 };
	float rotation2D = 0.0f;

public:
	ComponentCanvas* canvas = nullptr;
	ResourceTexture* texture = nullptr;
};

BE_END_NAMESPACE
#endif
