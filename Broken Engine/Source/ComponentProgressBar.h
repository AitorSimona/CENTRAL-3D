#ifndef __COMPONENTPROGRESSBAR_H__
#define __COMPONENTPROGRESSBAR_H__
#pragma once

#include "Component.h"
#include "Math.h"
#include "Color.h"

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
	void DrawPlane(Color color, float percentage = 100.0f);

	// UI Functions
	void Scale(float2 size) { size2D = size; }
	void Move(float2 pos) { position2D = pos; }
	void Rotate(float rot) { rotation2D = rot; }

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

	static inline Component::ComponentType GetType() { return Component::ComponentType::ProgressBar; };

	//Scripting function
	void SetPercentage(float p) { percentage = p; }
	float GetPercentage() { return percentage; }

public:
	bool visible = true;

	float2 size2D = { 2.5,0.5 };
	float2 position2D = { 0,0 };
	float rotation2D = 0.0f;

private:
	Color colorP1 = { 1.0f, 1.0f, 1.0f, 1.0f };
	Color colorP2 = { 1.0f, 1.0f, 1.0f, 1.0f };
	float percentage = 100.0f;

public:
	ComponentCanvas* canvas = nullptr;
	ResourceTexture* texture = nullptr;
};

BE_END_NAMESPACE
#endif
