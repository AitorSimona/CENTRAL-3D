#ifndef __BE_COLOR_H__
#define __BE_COLOR_H__

#include "BrokenCore.h"

BE_BEGIN_NAMESPACE

struct BROKEN_API Color 
{
	float r, g, b, a;

	Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) 
	{
	}

	Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) 
	{
	}

	void Set(float r, float g, float b, float a = 1.0f) 
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	float* operator & () 
	{
		return (float*)this;
	}
};

extern Color BROKEN_API Red;
extern Color BROKEN_API Green;
extern Color BROKEN_API Blue;
extern Color BROKEN_API Black;
extern Color BROKEN_API White;

BE_END_NAMESPACE
#endif