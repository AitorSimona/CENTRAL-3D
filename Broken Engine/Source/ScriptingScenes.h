#ifndef __SCRIPTINGSCENES_H__
#define __SCRIPTINGSCENES_H__

#include "BrokenCore.h"

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingScenes {
public:
	ScriptingScenes();
	~ScriptingScenes();

public:
	void LoadSceneFromScript();
	void QuitGame();

};
BE_END_NAMESPACE
#endif // __SCRIPTINGSCENES_H__