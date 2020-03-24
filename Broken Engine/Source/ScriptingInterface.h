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
	void ChangeButtonColor();
	void ChangeImageColor();

	void SetBarPercentage(float percentage);
	void SetUIText(const char* text);
	void SetUITextAndNumber(const char* text, float number);
	void SetUITextNumber(float number);
};
BE_END_NAMESPACE
#endif // __SCRIPTINGINTERFACE_H__