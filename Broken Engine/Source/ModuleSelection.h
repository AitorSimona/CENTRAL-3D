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
	- Visual scene rectangle selection **NEW**
	- Script name in component title **NEW**
	- Selection can be both on hierarchy and scene:
		- Single -> mouse left click
		- Additive/Substractive -> mouse left click + CTRL
		- Multiple (weird on scene)-> mouse left click + SHIFT if there's at least one selected
	- Selected gameobjects can now:
		- Change parent to the dragged one
		- Add same component
		- Be highlighted both scene and hierarchy
		- Be deleted at once
		- Paste previously copied components 
		- Delete same component
	- TODO
		- Selection counter in inspector
		- Guizmo transformation to all
		
*/



BE_BEGIN_NAMESPACE

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

	void ApplyOBBTransformation();

	bool IsSelected(GameObject* gameobject);

	void HandleSelection(GameObject* gameobject);

	void UpdateAABB();

	GameObject* GetLastSelected() const;

	void ClearSelection();

	bool ComponentCanBePasted() const;

	const std::vector<GameObject*>* GetSelected() const { return &selection; }

	// Component management
	void CopyComponentValues(Component* component);

	void PasteComponentValues(Component* component);

	void PasteComponentValuesToSelected();

	void DeleteComponentToSelected();

	std::string component_name = "None";


	void Select(GameObject* gameobject);

	void UnSelect(GameObject* gameobject);

	void SceneRectangleSelect(float3 start);

private:

	void SelectLastTo(GameObject* gameobject);

	void SelectRecursive(GameObject* gameobject, GameObject* from, GameObject* to);


	bool ToggleSelect(GameObject* gameobject);

	void SelectIfIntersects();

public:

	const AABB& GetAABB() { return aabb_selection; }

private:

	std::vector<GameObject*> selection;
	
	GameObject* root = nullptr;

	// Shift selection
	bool start_selecting = false;
	bool stop_selecting = false;
	bool reverse_selecting = false;
	
	// Visual scene selection
	OBB aabb;
	AABB aabb_selection;
	bool is_rectangle_selection = false;
	float3 aabb_start = float3::zero;
	float3 aabb_end = float3::zero;

	// Component copy and paste
	json component_node;
	Component::ComponentType component_type = Component::ComponentType::Unknown;
};

BE_END_NAMESPACE
#endif