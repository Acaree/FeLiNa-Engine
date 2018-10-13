#ifndef _MODULE_H_
#define _MODULE_H_

#include "Globals.h"
#include "Parson/parson.h"
#include "Timer.h"

class Application;

class Module
{
private :
	bool enabled;

protected:

	char* name = "";

public:
	Application* App;

	Module(Application* parent, bool start_enabled = true) : App(parent)
	{}

	virtual ~Module()
	{}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Awake(JSON_Object* config = nullptr)
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
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

	virtual void SaveState(JSON_Object*) { };
	virtual void LoadState(JSON_Object*) { };
	inline char* GetName() const { return name; }

	PerfTimer module_timer;
	float last_update_ms=0;

};

#endif