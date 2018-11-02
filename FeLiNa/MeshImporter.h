#ifndef _MESH_IMPORTER_
#define _MESH_IMPORTER_

#include "Importer.h"
#include "GameObject.h"
#include "ModuleRenderer3D.h" //just for struct mesh, can be more clean
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"

struct Mesh;
class aiScene;
class aiNode;
struct aiLogStream;
struct aiMaterial;



class MeshImporter :public Importer
{
public:

	MeshImporter();
	~MeshImporter();

	//�LOAD CHECKERS?

	bool Import(const char* file_name, const char* file_path, std::string& output_file);
	bool Import(const void* buffer, uint size, std::string& output_file);

	void LoadModel(const aiScene* scene, aiNode* node, std::string& output_file, GameObject* obj, ComponentTransform* trans);

	void GenerateBufferData(Mesh* mesh_data);

	//bool Load(const char* file_name, Texture* output_texture);
	//bool Load(const void* buffer, uint size, Texture* output_texture);


};





#endif
