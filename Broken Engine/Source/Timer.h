#ifndef __TIMER_H__
#define __TIMER_H__

#include "SDL\include\SDL.h"
#include "BrokenCore.h"

BE_BEGIN_NAMESPACE
class BROKEN_API Timer {
public:

	// Constructor
	Timer();

	void Start();
	void Stop();

	Uint32 Read();

	const bool IsRunning() const { return running; }

private:

	bool	running;
	Uint32	started_at;
	Uint32	stopped_at;
};
BE_END_NAMESPACE
#endif //__TIMER_H__