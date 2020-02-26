#ifndef __BE_ENTRYPOINT_H__
#define __BE_ENTRYPOINT_H__

#include <stdlib.h>
#include "Application.h"
//#include "Globals.h"

#include "mmgr/mmgr.h"

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )

//Trick to tell AMD and NVIDIA drivers to use the most powerful GPU instead of a lower-performance (such as integrated) GPU
#ifdef _WIN32
//else: Use GPU by default.
extern "C" {
	// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

	// http://developer.amd.com/community/blog/2015/10/02/amd-enduro-system-for-developers/
	// or (if the 1st doesn't works): https://gpuopen.com/amdpowerxpressrequesthighperformance/
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

enum main_states {
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

extern BrokenEngine::Application* BrokenEngine::CreateApplication();
BrokenEngine::Application* App;

int main(int argc, char** argv) {
	ENGINE_AND_SYSTEM_CONSOLE_LOG("Starting app '%s'...", TITLE);

	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;


	while (state != MAIN_EXIT) {
		switch (state) {
		case MAIN_CREATION:

			ENGINE_AND_SYSTEM_CONSOLE_LOG("-------------- Application Creation --------------");
			App = BrokenEngine::CreateApplication();
			state = MAIN_START;
			break;

		case MAIN_START:

			ENGINE_AND_SYSTEM_CONSOLE_LOG("-------------- Application Init --------------");
			if (App->Init() == false) {
				ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: Application Init exits with ERROR");
				state = MAIN_EXIT;
			}
			else {
				state = MAIN_UPDATE;
				ENGINE_AND_SYSTEM_CONSOLE_LOG("-------------- Application Update --------------");
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR) {
				ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: Application Update exits with ERROR");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
		break;

		case MAIN_FINISH:

			ENGINE_AND_SYSTEM_CONSOLE_LOG("-------------- Application CleanUp --------------");
			if (App->CleanUp() == false) {
				ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: Application CleanUp exits with ERROR");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
	}
	ENGINE_AND_SYSTEM_CONSOLE_LOG("Exiting app '%s'...\n", TITLE);

	delete App;
	return main_return;
}


#endif