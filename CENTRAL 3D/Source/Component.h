#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"

class Component
{
public:
	enum class ComponentType
	{
		none = 0,
		mesh
	};

public:

	Component(Component::ComponentType type);
	virtual ~Component();

};

#endif

