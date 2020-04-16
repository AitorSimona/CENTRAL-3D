#ifndef _ERRORS_H_
#define _ERRORS_H_

#include "SDL/include/SDL.h"
#include "SDL/include/SDL_opengl_glext.h"

#include "glad/include/glad/glad.h"

#include <iostream>

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}


static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << ")" << " FILE: " << file << std::endl << "\t Line: " << line << "\t Function: " << function << std::endl;
		return false;
	}

	return true;
}


static void APIENTRY ErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	if (id == 131185 || id == 131218) //Don't know what error && shader being recompiled
		return;

	std::cout <<	"-----------------------------------------------------------------------" << std::endl <<
					"---------------------OPENGL ERROR CALLBACK START-----------------------" << std::endl;
	std::cout <<	"		Message: " << message << std::endl << "		Type: ";
	
	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:
			std::cout << "GL ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			std::cout << "DEPRECATED BEHAVIOR ERROR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			std::cout << "UNDEFINED BEHAVIOR ERROR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			std::cout << "PORTABILITY ERROR";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			std::cout << "PERFORMANCE ERROR";
			break;
		case GL_DEBUG_TYPE_OTHER:
			std::cout << "OTHER ERROR";
			break;
	}

	std::cout << std::endl << "		ID: " << id << std::endl << "		Severity: ";
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_LOW:
			std::cout << "LOW";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			std::cout << "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			std::cout << "HIGH";
			break;
	}

	std::cout << std::endl <<
		"---------------------OPENGL ERROR CALLBACK END--------------------------" << std::endl <<
		"-----------------------------------------------------------------------" << std::endl <<
		std::endl << std::endl << std::endl;
}


static void SetErrorHandler(int MajV, int minV)
{
	if (MajV < 4 || minV < 3) {

		std::cout << "\n\n\n\nglDebugMessageCallback not Available because of OpenGL Version!" << std::endl << "	Error Handler NOT running - Unsuccessful Operation" << std::endl;
		std::cout << "	OpenGL Version used by Error Handler: " << MajV << "." << minV << std::endl << std::endl << std::endl << std::endl;
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	// Enable the debug callback
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageCallback(ErrorCallback, 0);
	//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	//__debugbreak();
	// To Output a Message: glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Vary dangerous error");

	std::cout << std::endl << std::endl << std::endl << std::endl <<	"---- Registering OpenGL debug Callback - Error Handler ----" << std::endl << "	Error Handler Running Successfully" << std::endl;
	std::cout <<														"	Version used by Error Handler: " << MajV << "." << minV << std::endl << std::endl << std::endl << std::endl;
}

#endif