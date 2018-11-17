#ifndef _MODULE_RENDER_3D_
#define _MODULE_RENDER_3D_

#include "Module.h"
#include "Light.h"

#define MAX_LIGHTS 8


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
	bool Start();
	bool Awake(JSON_Object* config);
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	math::float4x4 perspective(float fovy, float aspect, float n, float f);

	void OnResize(int width, int height);

	void DrawCheckBoxEdgeGLPanel();

	void UpdateTransforms(GameObject* go);

	void DrawGameObjects(GameObject* go);
	
	void SaveState(JSON_Object* config);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;

	bool wire = false;
	bool debug_draw = false;

private:
	bool depth_test = false;
	bool cull_face = false;
	bool lighting = false;
	bool color_material = false;;
	bool texture2D = true;
	bool line_smooth = false;
	bool polygon_smooth = false;

};

#endif