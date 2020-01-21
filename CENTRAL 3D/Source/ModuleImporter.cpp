#include "ModuleImporter.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "Assimp/include/cimport.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "mmgr/mmgr.h"

void MyAssimpCallback(const char * msg, char * userData)
{
	// --- Get Assimp LOGS and print them to console ---
	CONSOLE_LOG("[Assimp]: %s", msg);
}

ModuleImporter::ModuleImporter(bool start_enabled) : Module(start_enabled)
{
}

ModuleImporter::~ModuleImporter()
{
}

bool ModuleImporter::Init(json file)
{
	// --- Stream LOG messages to MyAssimpCallback, that sends them to console ---
	struct aiLogStream stream;
	stream.callback = MyAssimpCallback;
	aiAttachLogStream(&stream);

	return true;
}

bool ModuleImporter::Start()
{
	//std::vector<std::string> filters;
	//filters.push_back("vertex");
	//filters.push_back("VERTEX");

	//ImportAssets(ASSETS_FOLDER, filters);

	return true;
}

void ModuleImporter::ImportAssets(const char * directory, std::vector<std::string>& filters)
{
		std::vector<std::string> files;
		std::vector<std::string> dirs;

		std::string dir((directory) ? directory : "");
		dir += "/";

		App->fs->DiscoverFiles(dir.c_str(), files, dirs);

		for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
		{
				ImportAssets((dir + (*it) + "/").c_str(), filters);
		}

		std::sort(files.begin(), files.end());

		for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
		{
			const std::string& str = *it;

			bool pass_filter = false;

			if (filters.size() > 0)
			{
				for (uint i = 0; i < filters.size(); ++i)
				{
					if (str.substr(str.find_last_of(".") + 1) == filters[i])
					{
						pass_filter = true;
						break;
					}
				}
			}

			if (pass_filter)
			{
				std::string path = directory;
				path.append((*it).data());
				//LoadFromPath(path.data());
			}
		}
}


bool ModuleImporter::CleanUp()
{
	// --- Detach assimp log stream ---
	aiDetachAllLogStreams();

	return true;
}