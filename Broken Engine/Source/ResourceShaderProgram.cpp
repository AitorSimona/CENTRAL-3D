#include "ResourceShaderProgram.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"


#include "mmgr/mmgr.h"

namespace BrokenEngine {
	ResourceShaderProgram::ResourceShaderProgram(uint UID, std::string source_file) : Resource(Resource::ResourceType::SHADER, UID, source_file) {
		// MYTODO: Should go to library folder?
		extension = ".shader";
		resource_file = SHADERS_FOLDER + std::to_string(UID) + extension;

		previewTexID = App->gui->defaultfileTexID;

	}

	ResourceShaderProgram::~ResourceShaderProgram() {
	}

	bool ResourceShaderProgram::LoadInMemory() {
		return true;

	}

	void ResourceShaderProgram::FreeMemory() {
	}

	void ResourceShaderProgram::OnOverwrite() {
	}

	void ResourceShaderProgram::OnDelete() {
		App->fs->Remove(resource_file.c_str());

		App->resources->RemoveResourceFromFolder(this);
		App->resources->ONResourceDestroyed(this);
	}
}
