#ifndef __SCRIPTINGSCENES_H__
#define __SCRIPTINGSCENES_H__

#include "BrokenCore.h"

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingScenes {
public:
	ScriptingScenes();
	~ScriptingScenes();

public:
	void LoadSceneFromScript(uint scene_UUID);
	void QuitGame();
	uint Instantiate(uint resource_UUID, float x, float y, float z, float alpha, float beta, float gamma);

};
BE_END_NAMESPACE
#endif // __SCRIPTINGSCENES_H__