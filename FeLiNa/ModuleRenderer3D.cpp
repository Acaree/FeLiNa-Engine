#include "Glew/include/glew.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleScene.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ComponentMaterial.h"
#include "ModuleResourceManager.h"
#include "ComponentMesh.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#ifndef GAME_MODE
#include "ModuleGui.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_opengl2.h"
#include "Quadtree.h"
#include "mmgr/mmgr.h"
#endif

#pragma comment (lib, "Glew/libx86/glew32.lib")
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Render";
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{
}

// Called before render is available
bool ModuleRenderer3D::Init()
{

	LOG("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	GLenum err = glewInit(); // … check for errors 
	
	if (err != GLEW_OK)
	{
		LOG("Glew library could not init %s\n", glewGetErrorString(err));
		ret = false;
	}

	if (ret == true)
	{

		LOG("Vendor: %s", glGetString(GL_VENDOR));
		LOG("Renderer: %s", glGetString(GL_RENDERER));
		LOG("OpenGL version supported %s", glGetString(GL_VERSION));
		LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		
		//Use Vsync
		(App->vsync) ? SDL_GL_SetSwapInterval(1) : SDL_GL_SetSwapInterval(0);
			
		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			//LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			//LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			//LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		lights[0].Active(true);

		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		

		(lighting) ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
		(line_smooth) ? glEnable(GL_LINE_SMOOTH) : glDisable(GL_LINE_SMOOTH);
		(polygon_smooth) ? glEnable(GL_POLYGON_SMOOTH) : glDisable(GL_POLYGON_SMOOTH);

		// By default : true
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_COLOR_MATERIAL);

	}

	return ret;
}

bool ModuleRenderer3D::Awake(JSON_Object* config)
{

	depth_test = json_object_get_boolean(config, "Depth test");
	cull_face = json_object_get_boolean(config, "Cull face");
	lighting = json_object_get_boolean(config, "Lighting");
	color_material = json_object_get_boolean(config, "Color material");
	texture2D = json_object_get_boolean(config, "Texture 2D");
	line_smooth = json_object_get_boolean(config, "Line smooth");
	polygon_smooth = json_object_get_boolean(config, "Polygon smooth");

	return true;
}

bool ModuleRenderer3D::Start()
{
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);
	return true;
}

void ModuleRenderer3D::SaveState(JSON_Object* config)
{

	json_object_set_boolean(config, "Depth test", depth_test);
	json_object_set_boolean(config, "Cull face", cull_face);
	json_object_set_boolean(config, "Lighting", lighting);
	json_object_set_boolean(config, "Color material", color_material);
	json_object_set_boolean(config, "Texture 2D", texture2D);
	json_object_set_boolean(config, "Line smooth", line_smooth);
	json_object_set_boolean(config, "Polygon smooth", polygon_smooth);
}

void ModuleRenderer3D::UpdateTransforms(GameObject* go) {

	if(go->transform != nullptr)
		go->transform->UpdateMatrix();

	for (int i = 0; i < go->GetNumChildren(); i++)
	{
		UpdateTransforms(go->GetChild(i));
	}
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	
	glLoadMatrixf(App->camera->current_camera->GetViewMatrix());
	
	lights[0].SetPos(App->camera->current_camera->frustum.pos.x, App->camera->current_camera->frustum.pos.y, App->camera->current_camera->frustum.pos.z);

	UpdateTransforms(App->scene->root_object);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();
	
	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;
	
	//Draw grid
	App->scene->DrawScene();

	//Draw all meshes if are active
	for (uint i = 0; i < App->scene->root_object->GetNumChildren(); ++i)
	{
		if( App->scene->root_object->GetChild(i)->IsActive())
			DrawGameObjects(App->scene->root_object->GetChild(i));
	}

	//Debug
#ifndef GAME_MODE
	if (debug_draw)
	{
		for (uint i = 0; i < App->scene->root_object->GetNumChildren(); ++i)
		{
			App->scene->root_object->GetChild(i)->DrawBoundingBox();
		}

		App->scene->root_object->GetChild(0)->camera->DebugDraw(); //always child 0 is camera.
		App->scene->quadtree->DebugDraw();
		
	}


	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

#endif

	//SDL_GL_MakeCurrent(App->window->window, context);
	SDL_GL_SwapWindow(App->window->window);


	(wire) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE): glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);		

#ifndef GAME_MODE
	if (App->gui->close_program)
		update_return = UPDATE_STOP;
#endif

	return update_return;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);
	
	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	float aspect_ratio = (float)width / (float)height;

	App->camera->current_camera->SetAspectRatio(aspect_ratio);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glLoadMatrixf(App->camera->current_camera->GetProjectionMatrix());
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

#ifndef GAME_MODE
void ModuleRenderer3D::DrawCheckBoxEdgeGLPanel()
{
	if (ImGui::Checkbox("DEPTH_TEST", &depth_test))
	{
		(!depth_test) ? glDisable(GL_DEPTH_TEST): glEnable(GL_DEPTH_TEST);
	}
	if (ImGui::Checkbox("CULL FACE", &cull_face))
	{
		(!cull_face) ? glDisable(GL_CULL_FACE) : glEnable(GL_CULL_FACE);

	}
	if (ImGui::Checkbox("LIGHTING", &lighting))
	{
		(!lighting) ? glDisable(GL_LIGHTING) : glEnable(GL_LIGHTING);
	}
	if (ImGui::Checkbox("COLOR MATERIAL", &color_material))
	{
		(!color_material) ? glDisable(GL_COLOR_MATERIAL) : glEnable(GL_COLOR_MATERIAL);
	}
	if (ImGui::Checkbox("TEXTURE 2D", &texture2D))
	{
		(!texture2D) ? glDisable(GL_TEXTURE_2D) : glEnable(GL_TEXTURE_2D);
	}
	if (ImGui::Checkbox("LINE SMOOTH", &line_smooth))
	{
		(!line_smooth) ? glDisable(GL_LINE_SMOOTH) : glEnable(GL_LINE_SMOOTH);

	}
	if (ImGui::Checkbox("POLYGON SMOOTH", &polygon_smooth))
	{
		(!polygon_smooth) ? glDisable(GL_POLYGON_SMOOTH) : glEnable(GL_POLYGON_SMOOTH);
	}

}
#endif

void ModuleRenderer3D ::DrawGameObjects(GameObject*go)
{
	glPushMatrix();
	if (go->mesh != nullptr)
	{
		ComponentMesh* mesh = go->mesh;
		ComponentTransform* trans = go->transform;
		ComponentMaterial* material = go->material;


		math::float4x4 matrix = trans->GetTransformMatrix();
		glMultMatrixf((GLfloat*)matrix.Transposed().ptr());


		if (material == nullptr)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			ResourceMaterial* texture = (ResourceMaterial*)App->resource_manager->Get(material->GetUID());
			if (texture == nullptr) {

				glBindTexture(GL_TEXTURE_2D, 0);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, texture->id_texture);
			}
		}

		ResourceMesh* m_mesh = (ResourceMesh*)App->resource_manager->Get(mesh->GetUID());

		if (m_mesh != nullptr)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, m_mesh->id_vertices);
			glVertexPointer(3, GL_FLOAT, 0, NULL);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, m_mesh->id_uv);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mesh->id_indices);

			glDrawElements(GL_TRIANGLES, m_mesh->num_indices, GL_UNSIGNED_INT, NULL);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		
	}
	glPopMatrix();
	for (uint i = 0; i < go->GetNumChildren(); ++i)
	{
		if (go->GetChild(i)->IsActive())
		{
			DrawGameObjects(go->GetChild(i));
		}
	}

}



