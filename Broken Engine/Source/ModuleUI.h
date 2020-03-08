#ifndef __MODULEUI_H__
#define __MODULEUI_H__
#pragma once
#include "Module.h"
#include <vector>
#include <map>
#include "Math.h"

BE_BEGIN_NAMESPACE



class ComponentCanvas;
class Component;
class BROKEN_API ModuleUI : public Module
{
public:

	struct Character {
		GLuint	TextureID;  // ID handle of the glyph texture
		float2	Size;       // Size of glyph
		float2	Bearing;    // Offset from baseline to left/top of glyph
		GLuint	Advance;    // Offset to advance to next glyph
	};

	std::map<GLchar, Character> characters;
	GLuint VAO, VBO;
public:
	ModuleUI(bool start_enabled = true);
	~ModuleUI();

	bool Init(json& file) override;
	bool Start() override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void Draw() const;
	void AddCanvas(ComponentCanvas* c) { canvas.push_back(c); }
	void RemoveCanvas(ComponentCanvas* c);
	
	void Clear();

	bool CheckMousePos(Component* element, SDL_Rect collider);
	bool CheckClick(Component* element, bool draggable);

public:
	std::vector<ComponentCanvas*> canvas;

	float2 drag_start = float2::zero;
	float2 mouse_pos = float2::zero;

};

BE_END_NAMESPACE
#endif

