#ifndef _MODULEGUI_H
#define _MODULEGUI_H

#ifndef GAME_MODE
#include "Module.h"
#include "ImGuizmo/ImGuizmo.h"

class ComponentTransform;
class Resource;

enum FILE_TYPE;
class ModuleGui : public Module
{
public:

	ModuleGui(Application* app, bool start_enabled = true);
	~ModuleGui();

	bool Start();
	bool Awake(JSON_Object* config);
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
	void SaveState(JSON_Object* config);

private:
	bool About_active = false;
	bool want_to_rename = false;
	bool creating_folder = false;

	std::string file_to_rename;

	void ShowMainMenuBar();
	void ShowConfigurationWindow();
	void ShowAboutWindow();
	void ShowEditorMenu();

	void ShowAssetsWindow();
	void RecurssiveShowAssets(const char* dir);

	void SaveScene();
	void LoadScene();
	void ShowImportOptions();
	void Rename();
	void CreateFolder();
	void ShowAssetsOptions(const char* file, const char* dir);
public:

	bool close_program = false;
	bool open_configuration = false;
	bool open_console = false;
	bool need_screenshoot = false; 
	bool inspector_open = false;

	bool serialization_save_scene = false;
	bool serialization_load_scene = false;

	std::string file_focus;
	FILE_TYPE type;
};
#endif

#endif
