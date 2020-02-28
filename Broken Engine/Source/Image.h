#pragma once
#include "Component.h"
#include "Canvas.h"

class ResourceTexture;


class Image : public Component
{
public:
	Image(GameObject* gameObject);
	virtual ~Image();

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

	float2 size2D = { 1,1 };
	float2 position2D = { 0,0 };
	float rotation2D = 0.0f;

public:
	Canvas* canvas = nullptr;
	ResourceTexture* texture = nullptr;
};

