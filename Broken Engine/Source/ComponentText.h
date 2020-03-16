#ifndef __COMPONENTTEXT_H__
#define __COMPONENTTEXT_H__
#pragma once

#include "Component.h"
//#include "TextRenderer.hpp"
#include "Color.h"
#include <string>
#include "Math.h"

#define DEFAULT_FONT_SIZE 12
#define MAX_TEXT_SIZE 256
BE_BEGIN_NAMESPACE
class ResourceTexture;
class ComponentCanvas;

class BROKEN_API ComponentText : public Component
{
public:
	ComponentText(GameObject* gameObject);
	virtual ~ComponentText();

	void Update() override;

	void LoadFont(const char* path, int size);

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

	float2 size2D = { 0.02f,0.02f };
	float2 position2D = { 0,0 };
	float rotation2D = 0.0f;

	Color color = White;

public:
	ComponentCanvas* canvas = nullptr;
	ResourceTexture* texture = nullptr;

	float font_size = DEFAULT_FONT_SIZE;

	std::string text = "SampleText";
	//std::string buffer = "SampleText";
	char buffer[MAX_TEXT_SIZE] = "SampleText";
};

BE_END_NAMESPACE
#endif
