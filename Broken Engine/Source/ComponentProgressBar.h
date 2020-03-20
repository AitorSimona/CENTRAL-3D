#ifndef __COMPONENTPROGRESSBAR_H__
#define __COMPONENTPROGRESSBAR_H__
#pragma once

#include "Component.h"
#include "Math.h"

BE_BEGIN_NAMESPACE

class ComponentCanvas;
class ResourceTexture;

class BROKEN_API ComponentProgressBar : public Component
{
public:
	ComponentProgressBar(GameObject* gameObject);
	virtual ~ComponentProgressBar();

	void Update() override;

	void Draw();
	void DrawPlane(float2 size);

	// UI Functions
	void Scale_P1(float2 size) { P1_size2D = size; }
	void Scale_P2(float2 size) { P2_size2D = size; }
	void Move(float2 pos) { position2D = pos; }
	void Rotate(float rot) { rotation2D = rot; }

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

public:
	bool visible = true;

	float2 P1_size2D = { 1,1 };
	float2 P2_size2D = { 1,1 };
	float2 position2D = { 0,0 };
	float rotation2D = 0.0f;

	

public:
	ComponentCanvas* canvas = nullptr;
	ResourceTexture* texture = nullptr;
};

BE_END_NAMESPACE
#endif
