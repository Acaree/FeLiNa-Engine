#ifndef _MESH_IMPORTER_
#define _MESH_IMPORTER_

#include "Importer.h"
#include "GameObject.h"
#include "ModuleRenderer3D.h" //just for struct mesh, can be more clean
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"
#include <map>
#include <string>
#include <list>

class aiScene;
class aiNode;
class aiMesh;
struct aiLogStream;
struct aiMaterial;
class Resource;
enum FILE_TYPE;
class Resources;
class ResourceMesh;

struct MeshSettings: public ImporterSettings //We use this for load/save .meta
{
	enum ProcessNode { TargetRealtime_MaxQuality = 0, TargetRealtime_Quality, TargetRealtime_Fast, ConvertToLeftHanded, Customized };
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

	bool Import(const char* file_name, const char* file_path, std::string& output_file, const MeshSettings* import_settings);
	bool Import(const void* buffer, uint size, std::string& output_file, const MeshSettings* import_settings, const char* file_path = nullptr);

	void LoadModel(const aiScene* scene, aiNode* node, GameObject* parent, ComponentTransform* trans);


	bool LoadFLN(const char* path, ResourceMesh* mesh);
	bool LoadFLN(const void* buffer, uint size, ResourceMesh* mesh);

	void CreateFileMeta(std::list<Resource*> resources, MeshSettings* settings);
	void ReadFileMeta(const char* file, MeshSettings* settings);
	void GetMeshSettingsFromMeta(const char* file, MeshSettings* settings);
	void SetMeshSettingsFromMeta(const char* file, MeshSettings* settings);
	void ShowMeshImport();
	void RefreshMeshOptions();

	uint SetPostProccesConfiguration(const MeshSettings* import_settings);

public:
	MeshSettings* mesh_settings = nullptr;
	std::map<const aiMesh*, uint> meshes_map;
};





#endif

