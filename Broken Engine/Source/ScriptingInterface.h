#ifndef __SCRIPTINGINTERFACE_H__
#define __SCRIPTINGINTERFACE_H__

#include "BrokenCore.h"

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingInterface {
public:
	ScriptingInterface();
	~ScriptingInterface();

public:
	void MakeUIComponentVisible(uint go_UUID, const char* comp_type);
	void MakeUIComponentInvisible(uint go_UUID, const char* comp_type);

	void SetBarPercentage(uint go_UUID, float percentage);
	void SetCircularBarPercentage(uint go_UUID, float percentage);
	void SetUIText(uint go_UUID, const char* text);
	void SetUITextAndNumber(uint go_UUID, const char* text, float number);
	void SetUITextNumber(uint go_UUID, float number);
};
BE_END_NAMESPACE
#endif // __SCRIPTINGINTERFACE_H__
