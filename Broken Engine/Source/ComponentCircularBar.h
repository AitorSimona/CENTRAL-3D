#ifndef __COMPONENTCIRCULARBAR_H__
#define __COMPONENTCIRCULARBAR_H__
#pragma once

#include "Component.h"
#include "Math.h"
#include "Color.h"

BE_BEGIN_NAMESPACE

class ComponentCanvas;
class ResourceTexture;

class BROKEN_API ComponentCircularBar : public Component
{
public:
	ComponentCircularBar(GameObject* gameObject);
	virtual ~ComponentCircularBar();

	void Update() override;

	void Draw();
	void DrawCircle(Color color, bool axis = 0, float percentage = 100.0f); //axis 0(X), 1(Y)

	// UI Functions
	void Scale(float2 size) { size2D = size; }
	void Move(float2 pos) { position2D = pos; }
	void Rotate(float rot) { rotation2D = rot; }

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

	static inline Component::ComponentType GetType() { return Component::ComponentType::CircularBar; };

	//Scripting function
	void SetPercentage(float p) { percentage = p; }
	float GetPercentage() { return percentage; }

public:
	bool visible = true;
	bool axis = 0;

	float2 size2D = { 2,2 };
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