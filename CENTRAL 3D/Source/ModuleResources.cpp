#include "ModuleResources.h"


#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "mmgr/mmgr.h"

void MyAssimpCallback(const char * msg, char * userData)
{
	LOG("[Assimp]: %s", msg);
}

bool ModuleResources::CleanUp()
{
	// detach log stream
	aiDetachAllLogStreams();

	return true;
}

ModuleResources::ModuleResources(Application * app, bool start_enabled) : Module(app,start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Init(json file)
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream.callback = MyAssimpCallback;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
	return true;
}

bool ModuleResources::Start()
{

	return true;
}
