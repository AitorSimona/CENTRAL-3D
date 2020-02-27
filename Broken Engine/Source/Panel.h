#ifndef __PANEL_H__
#define __PANEL_H__
#include "BrokenCore.h"

BE_BEGIN_NAMESPACE

class Application;
class BROKEN_API Panel {
public:

	Panel(char* title);
	virtual ~Panel();

	virtual bool Draw() = 0;

	virtual inline void OnOff();
	virtual inline void SetOnOff(bool set) { enabled = set; }
	virtual inline bool IsEnabled() const;
	virtual inline char* GetName() const { return name; }

protected:
protected:
	bool enabled = false;
	char* name = nullptr;
};
BE_END_NAMESPACE
#endif

