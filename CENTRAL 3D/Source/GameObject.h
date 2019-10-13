#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "Globals.h"

class GameObject
{

public:

	GameObject();
	virtual ~GameObject();

	// --- Getters ---
	uint GetUID() const;

private:
	// Unique Identifier
	uint UID = 0;
};

#endif

