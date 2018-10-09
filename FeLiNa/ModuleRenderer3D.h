#pragma once
#include "OpenGL.h"
#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include "ModuleImage.h"
#include "ModuleGui.h"
#include "ModuleFBX.h"
#include "Assimp/include/cfileio.h"
#include <list>
#include "Cube_ArraysandIndex.h"
#define MAX_LIGHTS 8



struct ModelData
{

	GLuint id_indices = 0;
	uint num_indices = 0;
	uint* indices = nullptr;

	GLuint id_vertices = 0;
	uint num_vertices = 0;
	float* vertices = nullptr;

	uint texture_id = 0;

	GLuint id_uv = 0;
	uint num_uv = 0;
	float* uv = nullptr;

	GLuint id_color = 0;
	uint num_color = 0;
	float* colors = nullptr;
	aiColor4D color_4D;



};


class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
	float4x4 perspective(float fovy, float aspect, float n, float f);

	void OnResize(int width, int height);

	void DrawCheckBoxEdgeGLPanel();

	void DrawMesh(ModelData *mesh);

	void AddDataMesh(ModelData* data_mesh);
	void DeleteAllDataMesh();
public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	float3x3 NormalMatrix;
	float4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

private:
	bool depth_test = false;
	bool cull_face = false;
	bool lighting = false;
	bool color_material = false;;
	bool texture2D = false;
	bool line_smooth = false;
	bool polygon_smooth = false;
	
	ModuleImage* img;
	std::list<ModelData*> data;


	//TEST TO CENTER CAMERA
	AABB* test_box = nullptr;
	Cube_ArraysandIndex* cube_test = nullptr;
};