#pragma once
#include "UI_Element.h"
#include "ResourceMaterial.h"
#include <vector>

class Canvas : public UI_Element
{
public:
	Canvas(GameObject* gameObject, UI_Element::Type type);
	~Canvas();

	void AddElement(UI_Element* element) { elements.push_back(element); }

	void Draw();

	void CreateInspectorNode() override;
	static inline UI_Element::Type GetType() { return UI_Element::Type::CANVAS; }

private:
	//ResourceTexture* texture = nullptr;
	std::vector<UI_Element*> elements;
};

