#include "Glew/include/glew.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_opengl2.h"

#include "ModuleImage.h"

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
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

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
	material_cheker = json_object_get_boolean(config, "Checker");
	depth_test = json_object_get_boolean(config, "Depth test");
	cull_face = json_object_get_boolean(config, "Cull face");
	lighting = json_object_get_boolean(config, "Lighting");
	color_material = json_object_get_boolean(config, "Color material");
	texture2D = json_object_get_boolean(config, "Texture 2D");
	line_smooth = json_object_get_boolean(config, "Line smooth");
	polygon_smooth = json_object_get_boolean(config, "Polygon smooth");


	return true;
}


// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{

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

	for (std::vector<ModelData*>::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		DrawMesh(*it);
	}

	if (App->gui->need_screenshoot)
	{
		img->TakeScreenshoot();
		App->gui->need_screenshoot = false;
	}

	img->TakeScreenGif(dt);

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(App->window->window);

	
	return UPDATE_CONTINUE;
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


void ModuleRenderer3D :: DrawMesh(ModelData* mesh) {

	glColor4f(mesh->color_4D.r, mesh->color_4D.g, mesh->color_4D.b, mesh->color_4D.a);

	if (no_texture)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else if (material_cheker)
	{
		glBindTexture(GL_TEXTURE_2D, checker_id);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, mesh->texture_id);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_uv);
	glTexCoordPointer(2,GL_FLOAT,0 , NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_indices);

	glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindTexture(GL_TEXTURE_2D, 0);

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
	
}

void ModuleRenderer3D::DeleteAllDataMesh()
{
	data.clear();
}


void ModuleRenderer3D::AddTextureData(uint id_texture, uint widht, uint height, int index)
{
	if (index == -1)
	{
		for (uint i = 0; i < data.size(); ++i)
		{
			data[i]->texture_id = id_texture;
			data[i]->texture_width = widht;
			data[i]->texture_height = height;
		}
	}
	else
	{
		data[index]->texture_id = id_texture;
		data[index]->texture_width = widht;
		data[index]->texture_height = height;
	}

}


void ModuleRenderer3D::DrawMeshInformation()
{
	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;//ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;


	ImGui::Begin("Inspector",false,window_flags);

	if (data.size() != 0)
	{
		ModelData * it = data[0];


		ImGui::Text("File name: %s", it->name.c_str());
		ImGui::Text("Path: %s", it->path.c_str());
		ImGui::Text("All the information is just for reading.");

		if (ImGui::CollapsingHeader("Transformation", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Position:");
			ImGui::Text("x: %f  y: %f  z: %f", it->position.x, it->position.y, it->position.z);

			float3 rot_euler = it->rotation.ToEulerXYZ();
			ImGui::Text("Rotation:");
			ImGui::Text("x: %f  y: %f  z: %f", rot_euler.x, rot_euler.y, rot_euler.z);

			ImGui::Text("Scale:");
			ImGui::Text("x: %f  y: %f  z: %f", it->scale.x, it->scale.y, it->scale.z);
		}

		if (ImGui::CollapsingHeader("Mesh Information", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Indices: %i", it->num_indices);

			ImGui::Text("Vertices: %i", it->num_vertices);

			ImGui::Text("Uv's: %i", it->num_uv);

			ImGui::Text("Triangles: %i", it->num_vertices / 3);
		}

		if (ImGui::CollapsingHeader("Material Material", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Checker Material", &material_cheker);
			ImGui::Checkbox("No Texture", &no_texture);

			if (!no_texture)
			{
				if (material_cheker)
				{
					ImGui::Image((ImTextureID)(checker_id), ImVec2(250, 250));

					ImGui::Text("Associate Texture");
				}
				else
				{
					ImGui::Text("Width: %i", it->texture_width);
					ImGui::SameLine();
					ImGui::Text("Height: %i", it->texture_height);
					ImGui::Image((ImTextureID)(it->texture_id), ImVec2(250, 250));
				}
			}
		}
	}
	

	ImGui::End();

}

void ModuleRenderer3D::CreateCheckers()
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
}


void ModuleRenderer3D::CleanAllDataModel()
{
	
	for(int i = 0; i < data.size(); ++i)
	{
	
		data[i]->name.clear();
		data[i]->path.clear();
		
		glDeleteBuffers(1, (GLuint*) &(data[i]->id_vertices));
		glDeleteBuffers(1, (GLuint*) &(data[i]->id_indices));
		glDeleteTextures(1, (GLuint*) &(data[i]->texture_id));
		glDeleteBuffers(1, (GLuint*) &(data[i]->id_uv));
		glDeleteBuffers(1, (GLuint*) &(data[i]->id_color));

		if (data[i]->indices != nullptr)
		{
			delete[] data[i]->indices;
			data[i]->indices = nullptr;
		}

		if (data[i]->vertices != nullptr)
		{
			delete[] data[i]->vertices;
			data[i]->vertices = nullptr;
		}

		if (data[i]->uv != nullptr)
		{
			delete[] data[i]->uv;
			data[i]->uv = nullptr;
		}
		
		if (data[i]->colors != nullptr)
		{
			delete[] data[i]->colors;
			data[i]->colors = nullptr;
		}

		delete data[i];
	}

	
}