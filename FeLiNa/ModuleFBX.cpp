#include "ModuleFBX.h"


ModuleFBX::ModuleFBX(Application*app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleFBX::~ModuleFBX()
{

}

bool ModuleFBX::CleanUp()
{
	return true;
}

bool ModuleFBX::LoadFbx(const char* path)
{
	return true;
}