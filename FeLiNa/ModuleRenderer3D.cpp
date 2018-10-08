
#include "Application.h"
#include "ModuleRenderer3D.h"

#pragma comment (lib, "Glew/libx86/glew32.lib")
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Render";
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	img = new ModuleImage(App->window->screen_surface->w, App->window->screen_surface->h);
	LOG_GLOBAL("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOG_GLOBAL("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	GLenum err = glewInit(); // � check for errors 
	
	if (err != GLEW_OK)
	{
		LOG_GLOBAL("Glew library could not init %s\n", glewGetErrorString(err));
		ret = false;
	}

	if (ret == true)
	{

		LOG_GLOBAL("Vendor: %s", glGetString(GL_VENDOR));
		LOG_GLOBAL("Renderer: %s", glGetString(GL_RENDERER));
		LOG_GLOBAL("OpenGL version supported %s", glGetString(GL_VERSION));
		LOG_GLOBAL("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		
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
			//LOG_GLOBAL("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			//LOG_GLOBAL("Error initializing OpenGL! %s\n", gluErrorString(error));
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
			//LOG_GLOBAL("Error initializing OpenGL! %s\n", gluErrorString(error));
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
		
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE); // TO CHANGE
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
	}


	LOG_GLOBAL("Vendor: %s", glGetString(GL_VENDOR));
	LOG_GLOBAL("Renderer: %s", glGetString(GL_RENDERER)); 
	LOG_GLOBAL("OpenGL version supported %s", glGetString(GL_VERSION)); 
	LOG_GLOBAL("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	depth_test = glIsEnabled(GL_DEPTH_TEST);
	cull_face = glIsEnabled(GL_CULL_FACE);
	lighting = glIsEnabled(GL_LIGHTING);
	color_material = glIsEnabled(GL_COLOR_MATERIAL);
	texture2D = glIsEnabled(GL_TEXTURE_2D);
	line_smooth = glIsEnabled(GL_LINE_SMOOTH);
	polygon_smooth = glIsEnabled(GL_POLYGON_SMOOTH);

	

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	module_timer.Start();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());
	
	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();
	
	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{

	for (std::list<ModelData*>::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		DrawMesh(*it);
	}

	if (App->gui->need_screenshoot)
	{
		img->TakeScreenshoot();
		App->gui->need_screenshoot = false;
	}

	img->TakeScreenGif(dt);

	if (cube_test != nullptr)
		cube_test->Render();
	

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(App->window->window);

	last_update_ms = module_timer.ReadMs();
	module_timer.Start();
	
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG_GLOBAL("Destroying 3D Renderer");
	img->~ModuleImage();
	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf((GLfloat*)ProjectionMatrix.ptr());
	

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

float4x4 ModuleRenderer3D::perspective(float fovy, float aspect, float n, float f)
{
	float4x4 Perspective;

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

	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	

	if (ImGui::Begin("GL Options", &App->gui->open_render_configuration, window_flags))
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

		
		ImGui::End();
	}
	
}


void ModuleRenderer3D :: DrawMesh(ModelData* mesh) {


	glEnableClientState(GL_VERTEX_ARRAY);

	glColor4f(mesh->color_4D.r, mesh->color_4D.g, mesh->color_4D.b, mesh->color_4D.a); //COLOR MATERIAL: AMBIENT

	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_indices);
	glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

}

void ModuleRenderer3D::AddDataMesh(ModelData* data_mesh)
{

	data.push_back(data_mesh);

	float3 min = { data_mesh->vertices[0], data_mesh->vertices[1], data_mesh->vertices[2] };
	float3 max = { data_mesh->vertices[0], data_mesh->vertices[1], data_mesh->vertices[2] };
	
	for (uint i = 3; i < data_mesh->num_vertices; i+=3)
	{
		if (data_mesh->vertices[i] < min.x)
		{
			min.x = data_mesh->vertices[i];
		}
		if (data_mesh->vertices[i] > max.x)
		{
			max.x = data_mesh->vertices[i];
		}

		if (data_mesh->vertices[i+1] < min.y)
		{
			min.y = data_mesh->vertices[i+1];
		}
		if (data_mesh->vertices[i+1] > max.y)
		{
			max.y = data_mesh->vertices[i+1];
		}

		if (data_mesh->vertices[i + 2] < min.z)
		{
			min.z = data_mesh->vertices[i + 2];
		}
		if (data_mesh->vertices[i + 2] > max.z)
		{
			max.z = data_mesh->vertices[i + 2];
		}
	}
	
	App->camera->CreateFocusObject(min, max);
	cube_test = new Cube_ArraysandIndex(min, max-min,Type_CubeArraysandIndex);
}

void ModuleRenderer3D::DeleteAllDataMesh()
{
	
	data.clear();
	

	
}