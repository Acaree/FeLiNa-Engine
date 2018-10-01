#include "Application.h"
#include "ModuleSceneIntro.h"


ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Scene";
}

ModuleSceneIntro::~ModuleSceneIntro()
{
}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG_GLOBAL("Loading Intro assets");
	bool ret = true;

	/* vertex array
	vertexs[0] = 1.0f;
	vertexs[1] = 1.0f;
	vertexs[2] = 0.0f;

	vertexs[3] = 0.0f;
	vertexs[4] = 1.0f;
	vertexs[5] = 0.0f;

	vertexs[6] = 1.0f;
	vertexs[7] = 1.0f;
	vertexs[8] = 1.0f;


	glGenBuffers(1, (GLuint*) &(my_id));
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 180 * 3, vertexs, GL_STATIC_DRAW);
	*/

	
	
	
	vertexs[0] = 1.0f;
	vertexs[1] = 1.0f;
	vertexs[2] = 0.0f;

	vertexs[3] = 0.0f;
	vertexs[4] = 1.0f;
	vertexs[5] = 0.0f;

	vertexs[6] = 1.0f;
	vertexs[7] = 1.0f;
	vertexs[8] = 1.0f;
	
	
	glGenBuffers(1, (GLuint*) &(my_id)); 
	glBindBuffer(GL_ARRAY_BUFFER, my_id); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, vertexs, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*180 * 3, vertexs, GL_STATIC_DRAW);

	
	glGenBuffers(1, (GLuint*) &(my_indices)); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices); 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)* 8, indices, GL_STATIC_DRAW);

	
	//Initial position camera.
	App->camera->Move(float3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(float3(0, 0, 0));

	//Create Axis Plane 
	grid_plane = new mPlane(0, 1, 0, 0);
	grid_plane->axis = true;
	grid_plane->is_grid = true;

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG_GLOBAL("Unloading Intro scene");

	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(App->renderer3D->context);
	SDL_DestroyWindow(App->window->window);
	SDL_Quit();

	return true;
}

update_status ModuleSceneIntro::PreUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;



	return update_return;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	/* vertex array
	glEnableClientState(GL_VERTEX_ARRAY); 
	glBindBuffer(GL_ARRAY_BUFFER, my_id); 
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_TRIANGLES, 0, 9); 
	glDisableClientState(GL_VERTEX_ARRAY);*/
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);


	/*Direct mode
	glLineWidth(2.0f);
	glBegin(GL_TRIANGLES);

	glColor3f(1.0f, 0.5f, 0.0f);
	
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);

	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);

	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);

	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);

	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);

	/*glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glColor3f(0.0f, 1.0f, 0.0f);    
	glVertex3f(1.0f, 1.0f, -1.0f);    
	glVertex3f(-1.0f, 1.0f, -1.0f);   
	glVertex3f(-1.0f, 1.0f, 1.0f);    
	glVertex3f(1.0f, 1.0f, 1.0f);   
	glColor3f(1.0f, 0.5f, 0.0f);    
	glVertex3f(1.0f, -1.0f, 1.0f);    
	glVertex3f(-1.0f, -1.0f, 1.0f);    
	glVertex3f(-1.0f, -1.0f, -1.0f);    
	glVertex3f(1.0f, -1.0f, -1.0f);    
	glColor3f(1.0f, 0.0f, 0.0f);       
	glVertex3f(1.0f, 1.0f, 1.0f);    
	glVertex3f(-1.0f, 1.0f, 1.0f);    
	glVertex3f(-1.0f, -1.0f, 1.0f);   
	glVertex3f(1.0f, -1.0f, 1.0f);    
	glColor3f(1.0f, 1.0f, 0.0f);    
	glVertex3f(1.0f, -1.0f, -1.0f);    
	glVertex3f(-1.0f, -1.0f, -1.0f);    
	glVertex3f(-1.0f, 1.0f, -1.0f);    
	glVertex3f(1.0f, 1.0f, -1.0f);    
	glColor3f(0.0f, 0.0f, 1.0f);    
	glVertex3f(-1.0f, 1.0f, 1.0f);    
	glVertex3f(-1.0f, 1.0f, -1.0f);    
	glVertex3f(-1.0f, -1.0f, -1.0f);    
	glVertex3f(-1.0f, -1.0f, 1.0f);    
	glColor3f(1.0f, 0.0f, 1.0f);    
	glVertex3f(1.0f, 1.0f, -1.0f);    
	glVertex3f(1.0f, 1.0f, 1.0f);    
	glVertex3f(1.0f, -1.0f, 1.0f);    
	glVertex3f(1.0f, -1.0f, -1.0f);    
	*/
	glEnd();


	return update_return;
}

update_status ModuleSceneIntro::PostUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	grid_plane->Render();

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	return update_return;
}

