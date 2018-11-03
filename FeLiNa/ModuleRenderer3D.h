#ifndef _MODULE_RENDER_3D_
#define _MODULE_RENDER_3D_

#include "Module.h"
#include "Light.h"
#include "Assimp/include/cfileio.h"
#include "MathGeoLib/Math/Quat.h"
#define MAX_LIGHTS 8

class ImageRecorder;
class ComponentMesh;
class Component;
class GameObject;
class ComponentTexture;

struct Mesh;

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	bool Awake(JSON_Object* config);
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	math::float4x4 perspective(float fovy, float aspect, float n, float f);

	void OnResize(int width, int height);

	void DrawCheckBoxEdgeGLPanel();

	void UpdateTransforms(GameObject* go);

	void DrawGameObjects(ComponentMesh* mesh);

	void AddDataMesh(ComponentMesh* data_mesh);
	void DeleteAllDataMesh();

	uint CreateCheckers();
	void CleanAllDataModel();
	void SaveState(JSON_Object* config);


public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	
	math::float4x4  ProjectionMatrix;

	std::vector<ComponentMesh*> meshes;
	bool wire = false;

	//TO REVISE;
	bool debug_draw = false;

private:
	bool depth_test = false;
	bool cull_face = false;
	bool lighting = false;
	bool color_material = false;;
	bool texture2D = false;
	bool line_smooth = false;
	bool polygon_smooth = false;
	
	ImageRecorder* img = nullptr;

	//Checkers
	uint checker_id = 0;


};

#endif