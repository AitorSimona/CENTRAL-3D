#ifndef __SELECTION_H__
#define __SELECTION_H__

#include "Module.h"
#include "GameObject.h"
#include "Imgui/imgui.h"

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

	void UnSelect(GameObject* gameobject);

	bool ToggleSelect(GameObject* gameobject);

	const std::vector<GameObject*>* GetSelected() const { return &selection; }

private:

	std::vector<GameObject*> selection;
	GameObject* root = nullptr;

	// --- Set node flags ---
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	//ImGuiTreeNodeFlags node_flags = base_flags;
	
};

BE_END_NAMESPACE
#endif