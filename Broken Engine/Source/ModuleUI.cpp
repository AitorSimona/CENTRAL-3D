#include "BrokenCore.h"
#include "Application.h"
#include "ModuleUI.h"
#include "ModuleInput.h"
#include "ModuleSceneManager.h"
#include "GameObject.h"
#include "ModuleGui.h"
#include "Component.h"
#include "ComponentCanvas.h"

#pragma comment( lib, "Freetype/lib/freetype.lib" )

#include "Freetype/include/ft2build.h"

#include FT_FREETYPE_H

#include "mmgr/mmgr.h"

using namespace Broken;

ModuleUI::ModuleUI(bool start_enabled) : Module(start_enabled)
{
	name = "UI System";
}

ModuleUI::~ModuleUI()
{
}

bool ModuleUI::Start()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, "Assets/Fonts/calibri.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 48);

	// Init of all chars of the font to the map of character textures
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	// try to remove when working
	/*glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture = 0;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			float2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			float2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 5, NULL, GL_DYNAMIC_DRAW);
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	// texCoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

bool ModuleUI::Init(json& file)
{
	return true;
}

update_status ModuleUI::PreUpdate(float dt)
{
	//for (GameObject* obj : App->scene_manager->GetRootGO()->childs) //all objects in scene
	//{
	//	if (obj->HasComponent(Component::ComponentType::UI_Element)) //if has ui component
	//	{
	//		UI_Element* element = (UI_Element*)obj->GetComponent(Component::ComponentType::UI_Element); //single component (change when able to have multiple components of same type)
	//		element->UpdateCollider(); //update colliders
	//		element->UpdateState(); //update state

	//		if (element->GetState() == DRAGGING)
	//		{
	//			element->position2D.x = App->input->GetMouseX();
	//			element->position2D.y = App->input->GetMouseY();
	//		}
	//	}
	//}
	return UPDATE_CONTINUE;
}

update_status ModuleUI::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleUI::CleanUp()
{
	return true;
}

void ModuleUI::Draw() const
{
	// change camera to ortographic
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, App->gui->sceneHeight, 0, App->gui->sceneWidth, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	
	// Draw UI
	for (int i = 0; i < canvas.size(); i++)
	{
		if (canvas[i]->visible)
			canvas[i]->Draw();
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void ModuleUI::RemoveCanvas(ComponentCanvas* c)
{
	for (std::vector<ComponentCanvas*>::iterator it = canvas.begin(); it != canvas.end(); ++it)
	{
		if(*it && (*it)->GetContainerGameObject() && (*it)->GetContainerGameObject()->GetUID() == c->GetContainerGameObject()->GetUID())
		{
			canvas.erase(it);
			break;
		}
	}
}

void ModuleUI::Clear()
{
	canvas.clear();
}

bool ModuleUI::CheckMousePos(Component* component, SDL_Rect collider)
{
	mouse_pos.x = App->input->GetMouseX();
	mouse_pos.y = App->input->GetMouseY();

	//mouse_pos.x -= App->editor->tab_viewport->pos_x + 7;
	//mouse_pos.y = math::Abs(mouse_pos.y - (App->editor->tab_viewport->pos_y + 26 + App->editor->tab_viewport->height));// -mouse_pos.y + App->editor->focused_panel->pos_x;

	SDL_Rect MouseCollider = { mouse_pos.x,mouse_pos.y,1,1 };
	if (SDL_HasIntersection(&MouseCollider, &collider))
		return true;

	return false;
}

bool ModuleUI::CheckClick(Component* component, bool draggable)
{
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		drag_start.x = App->input->GetMouseX();
		drag_start.y = App->input->GetMouseY();
		return true;
	}

	if (draggable && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
		return true;

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
		return false;

	return false;
}