#ifndef __SCRIPTINGINTERFACE_H__
#define __SCRIPTINGINTERFACE_H__

#include "BrokenCore.h"

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingInterface {
public:
	ScriptingInterface();
	~ScriptingInterface();

public:
	void MakeUIComponentVisible(const char* comp_type, uint go_UUID);
	void MakeUIComponentInvisible(const char* comp_type, uint go_UUID);

	void SetBarPercentage(float percentage, uint go_UUID);
	void SetCircularBarPercentage(float percentage, uint go_UUID);
	void SetUIText(const char* text, uint go_UUID);
	void SetUITextAndNumber(const char* text, float number, uint go_UUID);
	void SetUITextNumber(float number, uint go_UUID);
};
BE_END_NAMESPACE
#endif // __SCRIPTINGINTERFACE_H__
