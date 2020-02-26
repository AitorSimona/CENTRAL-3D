#include "Panel.h"

#include "mmgr/mmgr.h"

namespace BrokenEngine {
	Panel::Panel(char* name) : name(name) {}

	Panel::~Panel() {}

	void Panel::OnOff() {
		enabled = !enabled;
	}

	bool Panel::IsEnabled() const {
		return enabled;
	}
}
