#include "Glew/include/glew.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"
#include "ModuleScene.h"
#include "GameObject.h"

#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ComponentTexture.h"
#include "ComponentMesh.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_opengl2.h"
#include "Quadtree.h"
#include "ImageRecorder.h"
#include "mmgr/mmgr.h"

#include "MeshImporter.h"

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
	//RELEASE(img);

	/*for (uint i = 0; i < meshes.size(); ++i)
	{

		RELEASE_ARRAY(meshes[i]->vertices);
		RELEASE_ARRAY(meshes[i]->indices);
		RELEASE_ARRAY(meshes[i]->uv);
		RELEASE(meshes[i]);
	}*/

	meshes.clear();


}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	img = new ImageRecorder(App->window->screen_surface->w, App->window->screen_surface->h);
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
		if (App->vsync) {
			SDL_GL_SetSwapInterval(1);
		}

		else {
			SDL_GL_SetSwapInterval(0);
		}
			
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
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		lights[0].Active(true);


		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
	}


	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER)); 
	LOG("OpenGL version supported %s", glGetString(GL_VERSION)); 
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	// Projection matrix for
	//OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	if (depth_test)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	if (cull_face)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	
	if (lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	if (texture2D)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);

	if (line_smooth)
		glEnable(GL_LINE_SMOOTH);
	else
		glDisable(GL_LINE_SMOOTH);

	if (polygon_smooth)
		glEnable(GL_POLYGON_SMOOTH);
	else
		glDisable(GL_POLYGON_SMOOTH);

	CreateCheckers();

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
	

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(App->camera->current_camera->GetViewMatrix());
	
	lights[0].SetPos(App->camera->current_camera->frustum.pos.x, App->camera->current_camera->frustum.pos.y, App->camera->current_camera->frustum.pos.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();
	
	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	// Recalculate all objects transformations
	UpdateTransforms(App->scene->root_object);

	//Draw grid
	App->scene->DrawScene();

	//Draw all meshes if are active
	for (uint i = 0; i < meshes.size(); ++i)
	{
		if(meshes[i]->GetParent()->IsActive())
			DrawGameObjects(meshes[i]);
	}

	//Debug
	if (debug_draw)
	{
		for (uint i = 0; i < meshes.size(); ++i)
		{
			meshes[i]->GetParent()->DrawBoundingBox(); 
		}

		App->camera->main_camera->camera->DebugDraw();

		App->scene->quadtree->DebugDraw();
		
	}

	//Screenshot and gif-> TO REVISION NOT WORK
	if (App->gui->need_screenshoot)
	{
		img->TakeScreenshoot();
		App->gui->need_screenshoot = false;
	}

	img->TakeScreenGif(dt);

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	

	//SDL_GL_MakeCurrent(App->window->window, context);
	SDL_GL_SwapWindow(App->window->window);


	if (wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (App->gui->close_program)
		update_return = UPDATE_STOP;

	
	return update_return;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");
	delete img;
	SDL_GL_DeleteContext(context);
	

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	float aspect_ratio = (float)width / (float)height;
	App->camera->main_camera->camera->SetAspectRatio(aspect_ratio);

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//ProjectionMatrix = App->camera->main_camera->camera->GetViewMatrix();
	glLoadMatrixf(App->camera->main_camera->camera->GetProjectionMatrix());
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

math::float4x4 ModuleRenderer3D::perspective(float fovy, float aspect, float n, float f)
{
	math::float4x4 Perspective;

	float coty = 1.0f / tan(fovy * math::pi / 360.0f);

	Perspective = Perspective.zero;
	
	Perspective.v[0][0] = coty / aspect;
	Perspective.v[1][1] = coty;
	Perspective.v[2][2] = (n + f) / (n - f);
	Perspective.v[2][3] = -1.0f;
	Perspective.v[3][2] = 2.0f * n * f / (n - f);
	
	return Perspective;

}

void ModuleRenderer3D::DrawCheckBoxEdgeGLPanel()
{
	if (ImGui::Checkbox("DEPTH_TEST", &depth_test))
	{
		if (!depth_test)
			glDisable(GL_DEPTH_TEST);
		else
			glEnable(GL_DEPTH_TEST);

	}
	if (ImGui::Checkbox("CULL FACE", &cull_face))
	{
		if (!cull_face)
			glDisable(GL_CULL_FACE);
		else
			glEnable(GL_CULL_FACE);

	}
	if (ImGui::Checkbox("LIGHTING", &lighting))
	{

		if (!lighting)
			glDisable(GL_LIGHTING);
		else
			glEnable(GL_LIGHTING);


	}
	if (ImGui::Checkbox("COLOR MATERIAL", &color_material))
	{
		if (!color_material)
			glDisable(GL_COLOR_MATERIAL);
		else
			glEnable(GL_COLOR_MATERIAL);

	}
	if (ImGui::Checkbox("TEXTURE 2D", &texture2D))
	{
		if (!texture2D)
			glDisable(GL_TEXTURE_2D);
		else
			glEnable(GL_TEXTURE_2D);

	}
	if (ImGui::Checkbox("LINE SMOOTH", &line_smooth))
	{
		if (!line_smooth)
			glDisable(GL_LINE_SMOOTH);
		else
			glEnable(GL_LINE_SMOOTH);

	}
	if (ImGui::Checkbox("POLYGON SMOOTH", &polygon_smooth))
	{
		if (!polygon_smooth)
			glDisable(GL_POLYGON_SMOOTH);
		else
			glEnable(GL_POLYGON_SMOOTH);

	}

}


void ModuleRenderer3D ::DrawGameObjects(ComponentMesh* mesh) 
{

	ComponentTransform* trans = mesh->GetParent()->transform;
	ComponentTexture* texture = mesh->GetParent()->material;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	

	math::float4x4 matrix = trans->GetTransformMatrix();

	glMultMatrixf((GLfloat*)matrix.Transposed().ptr());


	Mesh* m_mesh = mesh->GetMesh();


	glBindTexture(GL_TEXTURE_2D, 0);

	if (texture == nullptr || texture->no_texture) {

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else if (texture->material_checker)
	{
		glBindTexture(GL_TEXTURE_2D, checker_id);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
	}

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

	glBindTexture(GL_TEXTURE_2D, 0);





	glPopMatrix();


}

void ModuleRenderer3D::AddDataMesh(ComponentMesh* data_mesh) 
{
	meshes.push_back(data_mesh);
}

void ModuleRenderer3D::DeleteAllDataMesh()
{
	meshes.clear();
}

uint ModuleRenderer3D::CreateCheckers()
{
	GLubyte checkImage[36][36][4];
	for (int i = 0; i < 36; i++) {
		for (int j = 0; j < 36; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &checker_id);
	glBindTexture(GL_TEXTURE_2D, checker_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 36, 36, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

	glBindTexture(GL_TEXTURE_2D, 0);

	return checker_id;
}


void ModuleRenderer3D::CleanAllDataModel()
{

	for(int i = 0; i < meshes.size(); ++i)
	{
		Mesh* mesh = meshes[i]->GetMesh();
		
		glDeleteBuffers(1, (GLuint*) &(mesh->id_vertices));
		glDeleteBuffers(1, (GLuint*) &(mesh->id_indices));
		//glDeleteTextures(1, (GLuint*) &(meshes[i]->texture_id));
		glDeleteBuffers(1, (GLuint*) &(mesh->id_uv));
		//glDeleteBuffers(1, (GLuint*) &(meshes[i]->id_color));

		if (mesh->indices != nullptr)
		{
			delete[] mesh->indices;
			mesh->indices = nullptr;
		}

		if (mesh->vertices != nullptr)
		{
			delete[] mesh->vertices;
			mesh->vertices = nullptr;
		}

		if (mesh->uv != nullptr)
		{
			delete[] mesh->uv;
			mesh->uv = nullptr;
		}
		
		/*if (meshes[i]->colors != nullptr)
		{
			delete[] meshes[i]->colors;
			meshes[i]->colors = nullptr;
		}*/

		delete meshes[i];
	}
	meshes.clear();
	
}


