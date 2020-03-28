#ifndef __SELECTION_H__
#define __SELECTION_H__

#include "Module.h"
#include "GameObject.h"
//#include "Imgui/imgui.h"

/*
	I use this to not include the whole library of imgui
	ImGuiTreeNodeFlags_Selected 1
	ImGuiTreeNodeFlags_OpenOnArrow 128
	ImGuiTreeNodeFlags_SpanAvailWidth 2048
*/

/* CHANGE LIST
	- Added ModuleSelection to manage selection
	- Selection can be:
		- Single -> mouse left click
		- Additive/Substractive -> mouse left click + CTRL
		- Multiple -> mouse left click + SHIFT if there's at least one selected and from up to down
	- Selected gameobjects can now:
		- Change parent to the dragged one (WORKING ON IT)
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
	void SelectIfIntersects();
	update_status PostUpdate(float dt);

public:

	bool IsSelected(GameObject* gameobject);

	void HandleSelection(GameObject* gameobject);

	GameObject* GetLastSelected() const;

	void ClearSelection();

	const std::vector<GameObject*>* GetSelected() const { return &selection; }

private:

	void Select(GameObject* gameobject);

	void SelectLastTo(GameObject* gameobject);

	void SelectRecursive(GameObject* gameobject, GameObject* from, GameObject* to);

	void UnSelect(GameObject* gameobject);

	bool ToggleSelect(GameObject* gameobject);


private:
	bool start_selecting = false;
	bool stop_selecting = false;

	bool aabb_selection = false;
	std::vector<GameObject*> selection;
	
	GameObject* root = nullptr;

	AABB aabb;
	// --- Set node flags ---
	//ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	//ImGuiTreeNodeFlags node_flags = base_flags;
	
};

BE_END_NAMESPACE
#endif