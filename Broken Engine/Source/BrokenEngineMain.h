#ifndef __BROKENENGINEMAIN_H__
#define __BROKENENGINEMAIN_H__
#pragma once
#include <stdlib.h>
//#include "BrokenCore.h"
#include "Application.h"
#include "EngineLog.h"


#define NOMINMAX
#include <Windows.h>

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )

//#include "mmgr/mmgr.h"

////Trick to tell AMD and NVIDIA drivers to use the most powerful GPU instead of a lower-performance (such as integrated) GPU
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

#define IMGUI_DISABLE_DEFAULT_ALLOCATORS   

enum main_states {
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

extern Broken::Application* Broken::CreateApplication();

Broken::Application* mainApp = NULL;
int main(int argc, char** argv) {
	EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("Starting app...");

	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;

	while (state != MAIN_EXIT) {
		switch (state) {
		case MAIN_CREATION:

			EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("-------------- Application Creation --------------");
			mainApp = Broken::CreateApplication();
			state = MAIN_START;
			break;

		case MAIN_START:

			EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("-------------- Application Init --------------");
			if (mainApp->Init() == false) {
				EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: Application Init exits with ERROR");
				state = MAIN_EXIT;
			}
			else {
				state = MAIN_UPDATE;
				EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("-------------- Application Update --------------");
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = Broken::App->Update();

			if (update_return == UPDATE_ERROR) {
				EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: Application Update exits with ERROR");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
		break;

		case MAIN_FINISH:

			EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("-------------- Application CleanUp --------------");
			if (mainApp->CleanUp() == false) {
				EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("|[error]: Application CleanUp exits with ERROR");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
	}
	EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("Exiting app %s...\n", mainApp->GetAppName());

	delete mainApp;
	return main_return;
}

#endif
