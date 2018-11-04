#ifndef _APPLICATION_
#define _APPLICATION_

#include "Module.h"
#include "MathGeoLib/Algorithm/Random/LCG.h"
#include <list>
#include <vector>
#include "MaterialImporter.h"
#include "MeshImporter.h"


class ModuleHardware;
class ModuleWindow;
class ModuleInput;
class ModuleScene;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleGui;
class ModuleConsole;
class ModuleImport;
class ModuleFileSystem;
class ModuleTexture;
class ModuleTimeManagement;

enum ENGINE_STATES
{
	ENGINE_STATE_EDITOR_MODE = 0,
	ENGINE_STATE_GAME_MODE
};

enum GAME_STATES
{
	ENGINE_STATE_PLAY = 0,
	ENGINE_STATE_PAUSE,
	ENGINE_STATE_TICK,
	ENGINE_STATE_DEFAULT//Advance.
};



class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleScene* scene;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleGui* gui;
	ModuleHardware* hardware;
	ModuleConsole* console;
	ModuleImport * mesh_import;
	ModuleFileSystem* fs;
	ModuleTexture* texture;
	ModuleTimeManagement* time_management;
	//IMPORTERS
	MaterialImporter* importer_material ;
	MeshImporter* importer_mesh;

	char* app_name = nullptr;
	char* organization = nullptr;

	int FPS_cap = 60;

	bool vsync = false;
	math::LCG* random = nullptr;

	ENGINE_STATES engine_states = ENGINE_STATES::ENGINE_STATE_EDITOR_MODE;
	GAME_STATES game_states = GAME_STATES::ENGINE_STATE_DEFAULT;

private:
	char* name;
	PerfTimer	ms_timer;
	float	dt = 0.0f;
	double last_FPS = 0.0f;
	double last_ms = 0.0f;
	std::list<Module*>list_modules;

	std::vector<float> vector_ms;
	std::vector<float> vector_fps;
	std::vector<float> vector_memory;

	bool need_save = false;
	bool need_load = false;

	bool pause_diagram = false;
public:

	Application();
	~Application();

	bool Init();
	bool Awake();
	update_status Update();
	bool CleanUp();

	float GetMS() const;
	float GetFPS() const;

	void DrawApplicationInformationPanel();
	void DrawModulesTime();

	void Log_app(const char* text) const;

	void NeedSave();
	void NeedLoad();
private:

	void Save();
	void Load();

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;

#endif