#ifndef _MODULE_IMPORT_
#define _MODULE_IMPORT_

#include "Module.h"

class aiScene;
class aiNode;
struct aiLogStream;
struct aiMaterial;
class GameObject;
struct Mesh;
class ComponentTexture;
class ComponentTransform;

class ModuleImport : public Module
{
public:
	ModuleImport(Application* app, bool start_enabled = true);
	~ModuleImport();

	bool Start();
	bool CleanUp();
	
	bool Import(const char* importFileName, const char* importPath, std::string& outputFileName);
	bool LoadData(const char* path);
	void LoadModel(const aiScene* scene, aiNode* node, const char* path, GameObject* obj, ComponentTransform* trans);
	void GenerateBufferData(Mesh* mesh_data);
	ComponentTexture* FindTexturePath(aiMaterial* material, const char* path, int index );

};






#endif