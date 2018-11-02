#include "ModuleGui.h"
#include "Application.h"
#include "ModuleConsole.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleHardware.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl2.h"

#include "mmgr/mmgr.h"

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
	ImGui_ImplOpenGL2_Init();

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

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return update_return;
}

update_status ModuleGui::Update(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
			open_console = !open_console;

		if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
			open_configuration = !open_configuration;

		if (App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
			inspector_open = !inspector_open;
	}

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		need_screenshoot = true;
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		App->renderer3D->wire = !App->renderer3D->wire;


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

	if(open_configuration)
		ShowConfigurationWindow();

	if (open_console)
		App->console->DrawConsole();

	/*if (inspector_open)
		App->renderer3D->DrawMeshInformation();*/ //TO DELETE

	if (About_active) {
		ShowAboutWindow();
	}
	
	return update_return;
}

update_status ModuleGui::PostUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;
	
	


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

			if (ImGui::MenuItem("Take Screenshot", "1", false, true))
			{
				need_screenshoot = true;
			}

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

			if (ImGui::MenuItem("Wireframe", "2", App->renderer3D->wire, true))
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
	ImGui::SetNextWindowSize({400,800});
	
	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
	

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

		ImGui::Text("Game/");//TO Changes

		static char save_name[50];
		ImGui::InputText("###scene_name", save_name, 50);//default buffer size?¿

		if (ImGui::Button("Save Scene", ImVec2(100, 0)))
		{
			App->scene->serialization_scene.save_name_scene = save_name;
			ImGui::CloseCurrentPopup();
			App->scene->serialization_scene.SaveScene();
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

		ImGui::Text("Game/");//TO Changes

		static char name_load_scene[50]; // Same that saveScene, best default buf size??

		ImGuiInputTextFlags flag = ImGuiInputTextFlags_EnterReturnsTrue;
		ImGui::PushItemWidth(100.0f);
		

		ImGui::InputText("###scene_name", name_load_scene, 50, flag);//default buffer size?¿

		if (ImGui::Button("Load", ImVec2(100, 0)))
		{
			ImGui::CloseCurrentPopup();

			App->scene->serialization_scene.ClearActualScene();
			App->scene->serialization_scene.LoadScene(name_load_scene);
			//clear and load with path
			
			
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

