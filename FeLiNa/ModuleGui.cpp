#ifndef GAME_MODE
#include "ModuleGui.h"
#include "Application.h"
#include "ModuleConsole.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleHardware.h"
#include "ModuleScene.h"
#include "MaterialImporter.h"
#include "MeshImporter.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "ModuleTimeManagement.h"
#include "ModuleFileSystem.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "Resource.h"
#include "ImGuizmo/ImGuizmo.h"
#include "ModuleResourceManager.h"
#include "ComponentCamera.h"
#include "mmgr/mmgr.h"
#endif

#ifndef GAME_MODE



ModuleGui::ModuleGui(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Gui";
}

ModuleGui::~ModuleGui()
{
}

bool ModuleGui::Start()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	ImGui::StyleColorsDark();

	return true;
}

bool ModuleGui::Awake(JSON_Object* config)
{
	open_configuration = json_object_get_boolean(config, "Configuration");
	open_console = json_object_get_boolean(config, "Console");

	return true;
}

void ModuleGui::SaveState(JSON_Object* config)
{
	json_object_set_boolean(config, "Configuration",open_configuration);
	json_object_set_boolean(config, "Console", open_console);
}
update_status ModuleGui::PreUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	return update_return;

}

update_status ModuleGui::Update(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	ShowEditorMenu();

	
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
			open_console = !open_console;

		if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
			open_configuration = !open_configuration;

		if (App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
			inspector_open = !inspector_open;
	}

	if (App->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
		App->renderer3D->wire = !App->renderer3D->wire;


	ShowAssetsWindow();

	ShowMainMenuBar();

	if (serialization_save_scene)
	{
		ImGui::OpenPopup("Save Scene as:");
		SaveScene();
	}

	if (serialization_load_scene)
	{
		ImGui::OpenPopup("Load Scene:");
		LoadScene();
	}

	if (want_to_rename)
	{
		ImGui::OpenPopup("Rename as:");
		Rename();
	}

	if (creating_folder)
	{
		ImGui::OpenPopup("Folder name:");
		CreateFolder();
	}

	if (open_configuration)
		ShowConfigurationWindow();

	if (open_console)
		App->console->DrawConsole();

	if (About_active) {
		ShowAboutWindow();
	}


#ifndef GAME_MODE
	App->scene->ShowHierarchy();

	if (show_import_settings)
		ShowImportOptions();
	else
		App->scene->ShowInspector();
#endif		

	

	return update_return;
}



bool ModuleGui::CleanUp()
{
	

	return true;
}

void ModuleGui::ShowMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{

			if(ImGui::MenuItem("Save Scene", NULL, false, true))
			{
				serialization_save_scene = true;
			}

			if (ImGui::MenuItem("Load Scene", NULL, false, true))
			{
				serialization_load_scene = true;
			}

			if (ImGui::MenuItem("Close FeLiNa", NULL, false, true))
			{
				close_program = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Mode"))
		{

			if (ImGui::MenuItem("Wireframe", "H", App->renderer3D->wire, true))
			{
				App->renderer3D->wire = !App->renderer3D->wire;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Configuration", "Shift+M", open_configuration, true))
			{
				open_configuration = !open_configuration;
			}

			if (ImGui::MenuItem("Inspector", "Shift+I", inspector_open, true))
			{
				inspector_open = !inspector_open;
			}

			if (ImGui::MenuItem("Console", "Shift+C", open_console, true))
			{
				open_console = !open_console;
			}

			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu("Help"))
		{

			if (ImGui::MenuItem("Documentation", NULL, false, true))
			{
				ShellExecute(NULL, "open", "https://github.com/Acaree/FeLiNa-Engine", NULL, NULL, SW_SHOWNORMAL);
			}

			if (ImGui::MenuItem("Download", NULL, false, true))
			{
				ShellExecute(NULL, "open", "https://github.com/Acaree/FeLiNa-Engine/releases", NULL, NULL, SW_SHOWNORMAL);
			}

			if (ImGui::MenuItem("Report a bug", NULL, false, true))
			{
				ShellExecute(NULL, "open", "https://github.com/Acaree/FeLiNa-Engine/issues", NULL, NULL, SW_SHOWNORMAL);
			}

			if (ImGui::MenuItem("About", NULL, false, true))
			{
				About_active = !About_active;	
			}


			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}


void ModuleGui::ShowConfigurationWindow()
{
	ImGui::SetNextWindowPos(ImVec2(0, 10));
	
	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	

	ImGui::Begin("Configuration",&open_configuration, window_flags);

	if (ImGui::CollapsingHeader("Application"))
	{
		App->DrawApplicationInformationPanel();
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		App->window->DrawWindowInformationPanel();
	}
	if (ImGui::CollapsingHeader("Hardware Detection"))
	{
		App->hardware->DrawHardwareInformationPanel();
	}
	if (ImGui::CollapsingHeader("Render"))
	{
		App->renderer3D->DrawCheckBoxEdgeGLPanel();
	}
	if (ImGui::CollapsingHeader("Input"))
	{
		App->input->DrawInputConfiguration();
	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		App->camera->camera_editor->DrawInspector();
	}

	if (ImGui::CollapsingHeader("Modules Update Time"))
	{
		App->DrawModulesTime();
	}


	ImGui::End();
}


void ModuleGui::ShowAboutWindow() {

	
	ImGuiWindowFlags window_flags = 0;

	
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
	

	ImGui::Begin("About",&About_active, window_flags);

	ImGui::Text("FeLiNa Engine.");
	ImGui::Separator();
	ImGui::Text("This is a 3D game engine developed by two students\n from CITM-UPC Terrasa.");
	ImGui::Spacing();
	ImGui::Text("Authors:");
	if(ImGui::Button("Alfonso Sanchez-Cortes"))
		ShellExecute(NULL, "open", "https://github.com/Siitoo", NULL, NULL, SW_SHOWNORMAL);
	if (ImGui::Button("Alex Campamar"))
		ShellExecute(NULL, "open", "https://github.com/Acaree", NULL, NULL, SW_SHOWNORMAL);
	ImGui::Separator();
	ImGui::Text("Libraries:");


	ImGui::Text("ImGuI, Glew, OpenGL, SDL, \nSDL MathGeoLib, PCG, Parson, MMGR \nSDL Devil, Assimp \nSDL Window->Configuration->Hardware Detection to see software versions ");
	ImGui::Separator();

	ImGui::Text("MIT License Copyright(c) 2018 FeLiNa \n");
	ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
	ImGui::Text("of this software and associated documentation files(the %cSoftware%c), to deal", 34, 34);
	ImGui::Text("in the Software without restriction, including without limitation the rights");
	ImGui::Text("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell");
	ImGui::Text("copies of the Software, and to permit persons to whom the Software is");
	ImGui::Text("furnished to do so, subject to the following conditions :");
	ImGui::Text("The above copyright notice and this permission notice shall be included in all");
	ImGui::Text("copies or substantial portions of the Software.");
	ImGui::Text("THE SOFTWARE IS PROVIDED %cAS IS%c, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR", 34, 34);
	ImGui::Text("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
	ImGui::Text("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE");
	ImGui::Text("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
	ImGui::Text("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
	ImGui::Text("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE");
	ImGui::Text("SOFTWARE.");

	ImGui::End();

}

void ModuleGui::SaveScene()
{
	if (ImGui::BeginPopupModal("Save Scene as:", false, ImGuiWindowFlags_AlwaysAutoResize))
	{

		ImGui::Text("Scene will be save in: ");
		ImGui::Separator();

		ImGui::Text("Assets/");//TO Changes

		static char save_name[50];
		ImGui::InputText("###scene_name", save_name, 50);//default buffer size?¿

		if (ImGui::Button("Save Scene", ImVec2(100, 0)))
		{
			App->serialization_scene->save_name_scene = save_name;
			ImGui::CloseCurrentPopup();
			App->serialization_scene->SaveScene(App->scene->root_object);
			serialization_save_scene = false;
		}
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			serialization_save_scene = false;
		}

		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}

	
}

void ModuleGui::LoadScene()
{
	if (ImGui::BeginPopupModal("Load Scene:", false, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Scene will be searcher in: ");
		ImGui::Separator();

		ImGui::Text("Assets/");//TO Changes

		static char name_load_scene[50]; // Same that saveScene, best default buf size??

		ImGuiInputTextFlags flag = ImGuiInputTextFlags_EnterReturnsTrue;
		ImGui::PushItemWidth(100.0f);
		

		ImGui::InputText("###scene_name", name_load_scene, 50, flag);//default buffer size?¿
		ImGui::SameLine();
		ImGui::Text(".json");
		ImGui::Text("If you click LOAD the current scene will be deleted ");
		if (ImGui::Button("Load", ImVec2(100, 0)))
		{
			std::string file = "Assets/";
			file += name_load_scene;

			App->serialization_scene->ClearActualScene();
			App->scene->root_object->CleanData();
			App->scene->root_object->AddComponent(Component_Transform);
			App->serialization_scene->LoadScene((char*)file.c_str());
			ImGui::CloseCurrentPopup();

			// TO REVISE: clear and load with path
			
			
			serialization_load_scene = false;
		}
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			serialization_load_scene = false;
		}

		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}

}

void ModuleGui::ShowEditorMenu()
{

	static int width;
	static int height;
	SDL_GetWindowSize(App->window->window, &width, &height);

	ImGui::SetNextWindowPos(ImVec2(width / 2 - width/4, 17));
	ImGui::SetNextWindowSize(ImVec2(width/2 , 600 - height));


	ImGuiWindowFlags flags = 0;

	//ImGui::SetNextWindowSize({ 400,50 });

	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoCollapse;
	flags |= ImGuiWindowFlags_NoFocusOnAppearing;
	flags |= ImGuiWindowFlags_NoTitleBar;

	ImGui::Begin("##engine_editor",false, flags);

	bool a = false;
	
	if (ImGui::Button("Play", { 50,30 })) {
		App->time_management->PlayGameTime();
		App->game_states = GAME_STATES::ENGINE_STATE_PLAY;
		App->engine_states = ENGINE_STATES::ENGINE_STATE_GAME_MODE;
		App->serialization_scene->save_name_scene = "Autosave";
		App->serialization_scene->SaveScene(App->scene->root_object);
		App->camera->current_camera = App->scene->game_camera;
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause", { 50,30 })) {
		App->time_management->PauseGameClock();
		App->game_states = GAME_STATES::ENGINE_STATE_PAUSE;
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop", { 50,30 })) {

		App->time_management->StartGameTime();
		App->time_management->PauseGameClock();
		App->game_states = GAME_STATES::ENGINE_STATE_STOP;
		App->engine_states = ENGINE_STATES::ENGINE_STATE_EDITOR_MODE;
	
		

		App->scene->root_object->CleanData();

		App->scene->root_object->AddComponent(Component_Transform);

		App->serialization_scene->LoadScene();

		App->camera->current_camera = App->camera->camera_editor;

	}
	ImGui::SameLine();
	if (ImGui::Button("Tick", { 50,30 })) {
		App->time_management->tick_selected = true;
		App->time_management->PlayGameTime();
		App->game_states = GAME_STATES::ENGINE_STATE_TICK;
	}
	ImGui::SameLine();
	
	char game_time[20] = "";
	sprintf_s(game_time,sizeof(game_time), "%.3f", App->time_management->ReadGameClock());
	ImGui::Text(game_time);
	
	ImGui::SameLine();
	if(ImGui::Checkbox("Debug Draw", &App->renderer3D->debug_draw))
	{

	}
	
	ImGui::SameLine();

	ImGui::PushItemWidth(150);
	ImGui::SliderFloat("Time scale", &App->time_management->time_multiplier, 0.0f, 2.0f, "%.1f");


	ImGui::End();

	
}

void ModuleGui::ShowAssetsWindow()
{
	static int width;
	static int height;
	SDL_GetWindowSize(App->window->window, &width, &height);

	ImGui::SetNextWindowPos(ImVec2( 0 ,height - height/6 ));
	ImGui::SetNextWindowSize(ImVec2( width,height/6  ));
	ImGuiWindowFlags flags = 0;

	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoCollapse;
	flags |= ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin("Assets", false, flags))
	{
		if (ImGui::Button("Refreash Assets"))
		{
			App->fs->refresh_now = true;
		}
		if (ImGui::TreeNodeEx("Assets"))
		{
			RecurssiveShowAssets("Assets");
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Library"))
		{
			RecurssiveShowAssets("Library");
			ImGui::TreePop();
		}
	}

	ImGui::End();


}

void ModuleGui::RecurssiveShowAssets(const char* dir)
{
	ImGuiTreeNodeFlags flags = 0;
	
	static const char* tmp = "";
	static const char* tmp_dir = "";

	const char** directory_array = App->fs->GetAllFilesFromDir(dir);
	

	for (const char** file = directory_array; *file != nullptr; ++file)
	{
		
		if (App->fs->isDirectory(*file))
		{
			char* tmp = new char[DEFAULT_BUF_SIZE];
			flags = 0;
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;

			
			if (ImGui::TreeNodeEx(*file, flags))
			{
				strcpy(tmp, dir);
				strcat(tmp, "/");
				strcat(tmp, *file);
				
				RecurssiveShowAssets(tmp);

				
				
				ImGui::TreePop();
			}
		
			ShowAssetsOptions(*file, dir);

			RELEASE_ARRAY(tmp);

		}
		else
		{
			extension = *file;
			
			extension.erase(0,extension.find_last_of("."));

			if (strcmp(extension.c_str(),".meta") == 0)
				continue;

			flags = 0;
			flags |= ImGuiTreeNodeFlags_Leaf;


			if (ImGui::TreeNodeEx(*file, flags)) {
				if (ImGui::IsItemClicked(0))//Left click
				{

					file_focus = dir;
					file_focus += "/";
					file_focus += *file;

					if (ImGui::IsMouseDoubleClicked(0)) {
						std::string json_path = file_focus;
						int position = json_path.find(".json");
						if (position == std::string::npos) {
							json_path.erase(json_path.find_last_of("."), json_path.size());

							App->serialization_scene->LoadScene((char*)json_path.c_str());

							json_path.clear();
						}
					}
					else {

						file_dragging = file_focus;
					}
					
				}


				ShowAssetsOptions(*file, dir);
			
				ImGui::TreePop();
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				ImGui::SetDragDropPayload("Assets_Nodes", *file, strlen(*file));
				ImGui::EndDragDropSource();
			}
	
		}

	}

	file_to_rename = file_focus;

	App->fs->FreeEnumeratedFiles(directory_array);
}


void ModuleGui::ShowAssetsOptions(const char* file, const char* dir)
{
	if (ImGui::BeginPopupContextItem(file))
	{

		if (ImGui::MenuItem("See Import Options", NULL, false, false))
		{

			show_import_settings = true;

			file_focus = dir;
			file_focus += "/";
			file_focus += file;
			std::string tmp = file_focus;
			tmp += ".meta";

			type = App->fs->FindTypeFile(file_focus.c_str());

			switch (type)
			{
			case MESH_FILE:
				App->importer_mesh->GetMeshSettingsFromMeta(tmp.c_str(), App->importer_mesh->mesh_settings);
				break;
			case MATERIAL_FILE:
				App->importer_material->GetImportSettingsInMeta(tmp.c_str(), App->importer_material->material_settings);
				break;
			}


		}

		if (ImGui::MenuItem("Rename", NULL, false, true))
		{
			want_to_rename = true;

			file_focus = dir;
			file_focus += "/";
			file_focus += file;
			std::string tmp = file_focus;

			//if (directory)
				//App->fs->CreateFolder();

			file_to_rename = file_focus;
			//reimport
		}

		if (ImGui::MenuItem("Delete", NULL, false, true))
		{

			if(App->fs->isDirectory(file_focus.c_str())){
				std::string file_path;
				file_path += dir;
				file_path += "/";
				file_path += file;
				file_path += "/";
				App->fs->DeleteFolderandContainedFiles(file_path.c_str());
			
			}
			
			else {
				
				App->fs->RemoveAllDependencies((char*)file_focus.c_str());
				App->fs->FileDelete(file_focus.c_str());
			}
		}

		if (ImGui::MenuItem("Create new folder", NULL, false, true))
		{
			creating_folder = true;

		}
		ImGui::EndPopup();
	}


}

void ModuleGui::ShowImportOptions()
{
	static int width;
	static int height;
	SDL_GetWindowSize(App->window->window, &width, &height);

	ImGui::SetNextWindowPos(ImVec2(width - width / 4, 17));
	ImGui::SetNextWindowSize(ImVec2(width / 4, height - 400));

	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
	window_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
	window_flags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;

	ImGui::Begin("Inspector", &inspector_open, window_flags);
	ImGui::Text("Import options");
	ImGui::Separator();

	//uint uid = App->resource_manager->Find(file_focus.c_str());
	//Resource* resource = App->resource_manager->Get(uid);
	

	switch (type)
	{
	case MESH_FILE:
	{
		App->importer_mesh->ShowMeshImport();
	}
		break;

	case MATERIAL_FILE:
		App->importer_material->ShowMaterialImport();
		break;
	default:
		break;
	}

	ImGui::End();

}

void ModuleGui::Rename()
{
	if (ImGui::BeginPopupModal("Rename as:", false, ImGuiWindowFlags_AlwaysAutoResize))
	{

		static char new_save_name[50];
		ImGui::InputText("###scene_name", new_save_name, 50);//default buffer size?¿

		if (ImGui::Button("Rename", ImVec2(100, 0)))
		{
			std::string old_name = file_focus;

			//TO CHANGE ALL THIS ARE.. are.
			if (!App->fs->isDirectory(old_name.c_str()))
			{

				const char* tmp_file = old_name.erase(0, old_name.find_last_of("/") + 1).c_str();

				// TO CHANGE 
				std::string new_save_name_s = file_focus;
				std::string extension = file_focus;

				extension.erase(0, extension.find_last_of("."));

				new_save_name_s.erase(new_save_name_s.find_last_of("/") + 1, new_save_name_s.size());
				new_save_name_s += new_save_name;
				new_save_name_s += extension;

				rename(file_focus.c_str(), new_save_name_s.c_str());



				new_save_name_s += ".meta";

				old_name += ".meta";

				rename(old_name.c_str(), new_save_name_s.c_str());

				FILE_TYPE type = App->fs->FindTypeFile(file_focus.c_str());

				if (type == MESH_FILE) { //only fbxs generates a .json

					new_save_name_s.erase(new_save_name_s.find_first_of("."), new_save_name_s.size()); //remove extension and change it
					new_save_name_s += ".json";

					old_name.erase(old_name.find_first_of("."), old_name.size()); //remove extension and change it
					old_name += ".json";

					rename(old_name.c_str(), new_save_name_s.c_str());
				}
			}
			else
			{
				const char* tmp_file = old_name.erase(0, old_name.find_last_of("/") + 1).c_str();

				// TO CHANGE 
				std::string new_save_name_s = file_focus;
				new_save_name_s.erase(new_save_name_s.find_last_of("/") + 1, new_save_name_s.size());
				new_save_name_s += new_save_name;
			
				rename(file_focus.c_str(), new_save_name_s.c_str());
				
				App->fs->CreateFolder(new_save_name_s.c_str());
			}

			want_to_rename = false;
			
			file_focus.clear();
		}
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			want_to_rename = false;
			file_focus.clear();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}


}


void ModuleGui::CreateFolder()
{
	if (ImGui::BeginPopupModal("Folder name:",false, ImGuiWindowFlags_AlwaysAutoResize))
	{

		static char new_save_name[50];
		ImGui::InputText("###scene_name", new_save_name, 50);//default buffer size?¿

		if (ImGui::Button("Create", ImVec2(100, 0)))
		{
			std::string new_folder = "Assets/";

			new_folder += new_save_name;

			App->fs->CreateFolder(new_folder.c_str());

			creating_folder = false;

			file_focus.clear();
		}
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			creating_folder = false;
			file_focus.clear();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}


}




#endif