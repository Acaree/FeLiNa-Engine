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
enum FILE_TYPE;

struct MeshSettings: public ImporterSettings //We use this for load/save .meta
{
	enum ProcessNode { TargetRealtime_MaxQuality = 0, TargetRealtime_Quality, TargetRealtime_Fast, ConvertToLeftHanded };
	ProcessNode process_node = TargetRealtime_MaxQuality;
	
	bool CalcTangentSpace = true;
	bool JoinIdenticalVertices = true;
	bool MakeLeftHanded = false;
	bool Triangulate = true;
	bool RemoveComponent = false;
	bool GenNormals = false;
	bool GenSmoothNormals = true;
	bool SplitLargeMeshes = true;
	bool PreTransformVertices = false;
	bool LimitBoneWeights = true;
	bool ValidateDataStructure = true;
	bool RemoveRedundantMaterials = true;
	bool SortByPType = true;
	bool GenUvCoords = true;
	bool OptimizeMeshes = true;
	bool FlipUVs = false;
};


class MeshImporter :public Importer
{
public:

	MeshImporter();
	~MeshImporter();

	//¿LOAD CHECKERS?

	bool Import(const char* file_name, const char* file_path, std::string& output_file, const MeshSettings* import_settings);

	bool Import(const void* buffer, uint size, std::string& output_file, const MeshSettings* import_settings);

	void LoadModel(const aiScene* scene, aiNode* node, std::string& output_file, GameObject* obj, ComponentTransform* trans);


	Mesh* LoadFLN(const char* path);
	Mesh* LoadFLN(const void* buffer, uint size);

	void CreateFileMeta(Resource* resource, MeshSettings* settings);
	void ReadFileMeta(const char* file, MeshSettings* settings);

	void ShowMeshImport();
	void RefreshMeshOptions();

	uint SetPostProccesConfiguration(const MeshSettings* import_settings);

public:
	MeshSettings* mesh_settings = nullptr;

};





#endif

