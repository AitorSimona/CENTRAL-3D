#ifndef __SELECTION_H__
#define __SELECTION_H__

#include "Module.h"
#include "GameObject.h"
#include "Imgui/imgui.h"

/* CHANGE LIST
	- Added ModuleSelection to manage selection
	- Selection can be:
		- Single -> mouse left click
		- Additive/Substractive -> mouse left click + CTRL
		- Multiple -> mouse left click + SHIFT if there's at least one selected (WORKING ON IT)
	- Selected gameobjects can now:
		- Change parent to the dragged one
		- Add same component
		- Be highlighted both scene and hierarchy
		- Be deleted at once

*/
BE_BEGIN_NAMESPACE
// SELECTED TODO

class BROKEN_API ModuleSelection :	public Module
{
public:

	ModuleSelection(bool start_enabled = true);
	~ModuleSelection();
	bool Init(json& file);
	bool Start();
	bool CleanUp();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

public:

	bool IsSelected(GameObject* gameobject);

	GameObject* GetLastSelected() const;

	void ClearSelection();

	void Select(GameObject* gameobject);

	//void SelectLastTo(GameObject* gameobject);


	void UnSelect(GameObject* gameobject);

	bool ToggleSelect(GameObject* gameobject);

	const std::vector<GameObject*>* GetSelected() const { return &selection; }

	// PLEASE DO NOT EDIT
	// I think is the best short term approach to one click multi selection
	// I sacrifice memory over speed, as I'm duplicating the vector
	//std::vector<GameObject*> hierarchy_order;

private:
	//bool start_selecting = false;

	std::vector<GameObject*> selection;
	
	GameObject* root = nullptr;

	// --- Set node flags ---
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	//ImGuiTreeNodeFlags node_flags = base_flags;
	
};

BE_END_NAMESPACE
#endif