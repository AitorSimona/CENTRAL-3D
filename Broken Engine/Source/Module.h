#pragma once

#include <string>
#include "JSONLoader.h"

struct Event;

class Module
{
private :
	bool enabled;

public:

	Module(bool start_enabled = true) : enabled(start_enabled)
	{}

	virtual ~Module()
	{}

	virtual bool Init(json file) 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual void ONEvent(const Event& event) const {}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status GameUpdate(float gameDT)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual void SaveStatus(json &file) const {}

	virtual void LoadStatus(const json & file) {}

	virtual bool isEnabled() const 
	{
		return enabled;
	}

protected:

	std::string name = "Undefined";
};