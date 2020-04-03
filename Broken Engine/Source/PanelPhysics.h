#ifndef __PANEL_PHYSICS_H__
#define __PANEL_PHYSICS_H__

#include "PhysX_3.4/Include/PxPhysicsAPI.h"
#include "BrokenEngine.h"

class PanelPhysics : public Broken::Panel
{
public:

	PanelPhysics(char* name);
	~PanelPhysics();

	bool Draw();

	void CreateLayerFilterGrid();

	void CreateLayerList();

private:
	physx::PxVec3 gravity;
	float staticFriction, dynamicFriction, restitution;

	physx::PxVec3 tmpGravity;
	float tmpStaticFriction, tmpDynamicFriction, tmpRestitution; 
	float sizeMax = 0;

};
#endif