#ifndef __BE_ENTRYPOINT_H__
#define __BE_ENTRYPOINT_H__


#include "Application.h"

extern BrokenEngine::Application* BrokenEngine::CreateApplication();
BrokenEngine::Application* App = nullptr;

int main(int argc, char** argv);


#endif