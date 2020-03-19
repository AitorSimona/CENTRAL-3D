#ifndef __SCRIPTINGINTERFACE_H__
#define __SCRIPTINGINTERFACE_H__

#include "BrokenCore.h"

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingInterface {
public:
	ScriptingInterface();
	~ScriptingInterface();

public:
	void MakeUIComponentVisible();
	void ChangeUIText();
	void ChangeUITextWithNumber();
	void ChangeButtonColor();
	void ChangeImageColor();

};
BE_END_NAMESPACE
#endif // __SCRIPTINGINTERFACE_H__