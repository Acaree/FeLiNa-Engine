#ifndef _MESH_IMPORTER_
#define _MESH_IMPORTER_

#include "Importer.h"
#include "GameObject.h"
#include "ModuleRenderer3D.h" //just for struct mesh, can be more clean
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"

class aiScene;
class aiNode;
struct aiLogStream;
struct aiMaterial;
class Resource;

struct MeshSettings: public ImporterSettings //We use this for load/save .meta
{
	enum ProceesNode { TargetRealtime_MaxQuality = 0, TargetRealtime_Quality, TargetRealtime_Fast, ConvertToLeftHanded };
	ProceesNode procces_node = TargetRealtime_MaxQuality;
	//TO REVISE: TO COMPLETE.
};


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

	Mesh* LoadFLN(const char* path);
	Mesh* LoadFLN(const void* buffer, uint size);

	void CreateFileMeta(Resource* resource, MeshSettings* settings);
	void ReadFileMeta(const char* file, MeshSettings* settings);
	//bool Load(const char* file_name, Texture* output_texture);
	//bool Load(const void* buffer, uint size, Texture* output_texture);


};





#endif

