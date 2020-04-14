#ifndef __MODULE_GUI_H__
#define __MODULE_GUI_H__

#include "Module.h"
#include "BrokenCore.h"

#include <vector>
struct ImGuiContext;
struct ImDrawList;


typedef void* (*be_imguialloc)(size_t sz, void* user_data);
typedef void (*be_imguifree)(void* ptr, void* user_data);

BE_BEGIN_NAMESPACE
class ResourcePrefab;

class BROKEN_API ModuleGui : public Module
{
public:

	ModuleGui(bool start_enabled = true);
	~ModuleGui();

	bool Init(json& file) override;
	bool Start() override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool Stop() override;
	bool CleanUp() override;

	void Draw() const;
	void DockSpace() const;
	void RequestBrowser(const char * url) const;

	ImGuiContext* getImgUICtx() const;
	void HandleInput(SDL_Event* event) const;

	bool IsKeyboardCaptured() const;
	bool IsMouseCaptured() const;

	void CreateIcons();

	be_imguialloc GetImGuiAlloc() const;
	be_imguifree GetImGuiFree() const;

public:
	bool openPrefab = false;
	bool editingPrefab = false;
	ResourcePrefab* prefab = nullptr;


	uint folderTexID = 0;
	uint defaultfileTexID = 0;
	uint materialTexID = 0;
	uint prefabTexID = 0;
	uint playbuttonTexID = 0;
	uint sceneTexID = 0;
	uint animatorTexID = 0;
	uint navmeshTexID = 0;
	uint shaderTexID = 0;
	uint scriptTexID = 0;
	uint meshTexID = 0;
	uint boneTexID = 0;
	uint animationTexID = 0;
	// --- Toolbar icons
	uint translateTexID = 0;
	uint rotateTexID = 0;
	uint scaleTexID = 0;
	uint toolbarPlayTexID = 0;
	uint toolbarPauseTexID = 0;
	uint toolbarStepTexID = 0;

	bool isSceneHovered = false;

	//Scene position and size
	float sceneX = 0;
	float sceneY = 0;
	float sceneWidth = 0;
	float sceneHeight = 0;

	bool isUsingGuizmo = false;

	ImDrawList* draw_list;

private:

	bool capture_keyboard = false;
	bool capture_mouse = false;
	ImGuiContext* ctx = nullptr;
};

BE_END_NAMESPACE
#endif
