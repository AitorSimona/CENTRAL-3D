#ifndef __COMPONENTTEXT_H__
#define __COMPONENTTEXT_H__
#pragma once

#include "Component.h"
#include "Color.h"
#include <string>
#include "Math.h"

#define DEFAULT_FONT_SIZE 12
#define MAX_TEXT_SIZE 512

BE_BEGIN_NAMESPACE

class ResourceFont;
class ComponentCanvas;

class BROKEN_API ComponentText : public Component
{
public:
	ComponentText(GameObject* gameObject);
	virtual ~ComponentText();

	void Update() override;
	void Draw();

	static inline Component::ComponentType GetType() { return Component::ComponentType::Text; }

	// UI Functions
	void Scale(float2 size) { size2D = size; }
	void Move(float2 pos) { position2D = pos; }
	void Rotate(float rot) { rotation2D = rot; }

	void SetText(const char* new_text) { text = new_text; }

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void ONResourceEvent(uint UID, Resource::ResourceNotificationType type) override;
	void CreateInspectorNode() override;

public:

	bool visible = true;

	float2 position2D = { 0,0 };
	float rotation2D = 0.0f;
	float2 size2D = { 0.04f,0.04f };	

	Color color = White;

public:

	ComponentCanvas* canvas = nullptr;
	ResourceFont* font = nullptr;

private:

	// Try to change to char*
	std::string text = "SampleText";
	// Or try to change to string
	char buffer[MAX_TEXT_SIZE] = "SampleText";
};

BE_END_NAMESPACE
#endif
