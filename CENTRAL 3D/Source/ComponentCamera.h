#ifndef __COMPONENT_CAMERA_H__
#define __COMPONENT_CAMERA_H__

#include "Component.h"

class ComponentCamera : public Component
{
public:

	ComponentCamera(GameObject* ContainerGO);
	virtual ~ComponentCamera();

};

#endif