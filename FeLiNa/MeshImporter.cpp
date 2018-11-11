#include "MeshImporter.h"

#include "Glew/include/glew.h" 
#include "SDL/include/SDL_opengl.h"
#include "ModuleGui.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include "ModuleTimeManagement.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ResourceManager.h"
#include "MaterialImporter.h"
#include "Resource.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"


#pragma comment (lib,"Assimp/libx86/assimp.lib")

#include "mmgr/mmgr.h"
MeshImporter::MeshImporter()
{
	mesh_settings = new MeshSettings();
}


MeshImporter::~MeshImporter()
{
	RELEASE(mesh_settings);
}
//CONST????
bool MeshImporter::Import(const char* file_name, const char* file_path, std::string& output_file, const MeshSettings* import_settings)
{
	bool ret = false;

	if (file_name == nullptr || file_path == nullptr || import_settings == nullptr)
		return ret;
	
	char importer_path[DEFAULT_BUF_SIZE];
	strcpy_s(importer_path, strlen(file_path) + 1, file_path);

	if (file_name != nullptr)
		strcat_s(importer_path, strlen(importer_path) + strlen(file_name) + 1, file_name);

	std::string tmp = file_name;
	output_file = tmp.substr(0, tmp.find_last_of("."));

	char* buffer;
	uint size = App->fs->Load(importer_path, &buffer);

	if (size > 0)
	{
		LOG("MATERIAL IMPORTER: Successfully");
		ret = Import(buffer, size, output_file,import_settings);
		RELEASE_ARRAY(buffer);
	}	
	else
		LOG("MATERIAL IMPORTER: Could not load texture ");

	
	return ret;
}

// same that other this is const?
bool MeshImporter::Import(const void* buffer, uint size, std::string& output_file, const MeshSettings* import_settings)
{
	bool ret = false;

	LOG("Inicialization load data model");

	if (buffer == nullptr || import_settings == nullptr)
		return ret;

	uint flags = SetPostProccesConfiguration(import_settings);

	const aiScene* scene = aiImportFileFromMemory((const char*)buffer, size, flags, nullptr);

	if (scene != nullptr) {

		GameObject* childrens_go = new GameObject(nullptr);
		childrens_go->SetName((char*)output_file.data());

		ComponentTransform* trans = (ComponentTransform*)childrens_go->AddComponent(Component_Transform);

		aiNode* rootNode = scene->mRootNode;

		LoadModel(scene, rootNode, output_file, childrens_go, trans);

		
		//SITO TEST ZONE: WARNING THIS ARE A SHIT..
		App->serialization_scene->save_name_scene = childrens_go->GetName();
		App->serialization_scene->SaveScene(childrens_go);
		
		//RELEASE(childrens_go);
		//App->serialization_scene->ClearActualScene();
		//App->serialization_scene->save_name_scene = childrens_go->GetName();
		//App->serialization_scene->LoadScene(App->serialization_scene->save_name_scene);

		aiReleaseImport(scene);
		RELEASE(childrens_go);

		ret = true;
	}

	return ret;
}

void MeshImporter::LoadModel(const aiScene* scene, aiNode* node, std::string& output_file, GameObject* obj, ComponentTransform* trans)
{

	//Creating a game object to set data
	GameObject* game_object = new GameObject(nullptr);
	game_object->SetName(node->mName.data);

	aiQuaternion q;
	aiVector3D scale, pos;
	node->mTransformation.Decompose(scale, q, pos);

	ComponentTransform* component_transform = new ComponentTransform(nullptr, math::float3(pos.x, pos.y, pos.z), math::float3(q.GetEuler().x, q.GetEuler().y, q.GetEuler().z), math::float3(scale.x, scale.y, scale.z));
	component_transform->SumPosition(trans->GetPosition());
	component_transform->SumRotation(trans->GetRotation());
	component_transform->SumScale(trans->GetScale());


	if (node->mNumMeshes > 0)
	{

		output_file = node->mName.data;

		ComponentTransform* transform = (ComponentTransform*)game_object->AddComponent(Component_Transform);
		transform->SetPosition(component_transform->GetPosition());
		transform->SetRotation(component_transform->GetRotation());
		transform->SetScale(component_transform->GetScale());

		RELEASE(component_transform); // this last line of function?


		ComponentMesh* component_mesh = (ComponentMesh*)game_object->AddComponent(Component_Mesh);
		ComponentTexture* component_texture = (ComponentTexture*)game_object->AddComponent(Component_Material);


		Mesh* mesh_data = component_mesh->GetMesh();
		

		for (int num_meshes = 0; num_meshes < node->mNumMeshes; ++num_meshes)
		{
			aiMesh* new_mesh = scene->mMeshes[node->mMeshes[num_meshes]];

			//Load Vertices
			mesh_data->num_vertices = new_mesh->mNumVertices;
			mesh_data->vertices = new float[mesh_data->num_vertices * 3];
			memcpy(mesh_data->vertices, new_mesh->mVertices, sizeof(float)*mesh_data->num_vertices * 3);
			LOG("New mesh with %d vertices", mesh_data->num_vertices);

			//Load Mesh
			if (new_mesh->HasFaces())
			{
				mesh_data->num_indices = new_mesh->mNumFaces * 3;
				mesh_data->indices = new uint[mesh_data->num_indices];

				for (uint num_faces = 0; num_faces < new_mesh->mNumFaces; ++num_faces)
				{
					if (new_mesh->mFaces[num_faces].mNumIndices != 3)
					{
						LOG("Geometry face %i whit %i faces", num_faces, new_mesh->mFaces[num_faces].mNumIndices);
					}
					else
						memcpy(&mesh_data->indices[num_faces * 3], new_mesh->mFaces[num_faces].mIndices, 3 * sizeof(uint));

				}


			}

			//Texture
			aiMaterial* material = scene->mMaterials[new_mesh->mMaterialIndex];
			aiString name;
			
			material->GetTexture(aiTextureType_DIFFUSE, 0, &name);

			std::string file_path = name.data;

			std::string file_name = file_path.substr(file_path.find_last_of("\\") + 1, file_path.size());

			std::string path = file_path.erase(file_path.find_last_of("\\"), file_path.size());

			std::string texture_generated;
		
			char* file_name_c = new char[file_name.size() + 1];

			strcpy_s(file_name_c, file_name.size() + 1, file_name.data());

			char* path_c = new char[path.size() + 1];

			strcpy_s(path_c, path.size() + 1, path.data());

			char add [DEFAULT_BUF_SIZE] = "Assets";

			sprintf_s(add, DEFAULT_BUF_SIZE, "%s/%s/%s", add, path_c,file_name.data());



			MaterialSettings* settings = new MaterialSettings();
			App->importer_material->Import((const char*)add, texture_generated, settings);
			
			Texture * tex = new Texture();
			tex->felina_path = add;
			component_texture->SetTexture(tex);
			
			
			RELEASE_ARRAY(path_c);
			RELEASE_ARRAY(file_name_c);
			RELEASE(settings);

			file_path.clear();
			file_name.clear();
			path.clear();
			texture_generated.clear();

		
			
			if (new_mesh->HasTextureCoords(0))
			{
				mesh_data->num_uv = new_mesh->mNumVertices;
				mesh_data->uv = new float[mesh_data->num_uv * 2];

				for (uint coords = 0; coords < new_mesh->mNumVertices; ++coords)
				{
					memcpy(&mesh_data->uv[coords * 2], &new_mesh->mTextureCoords[0][coords].x, sizeof(float));
					memcpy(&mesh_data->uv[(coords * 2) + 1], &new_mesh->mTextureCoords[0][coords].y, sizeof(float));
				}


			}

			


		}

		//CREATE AABB
		game_object->AddBoundingBox(mesh_data);
		

		obj->AddChildren(game_object);
		game_object->RecalculateBoundingBox();

		/*******************************************************************/

		//put in a new function savefln



		uint ranges[3] = { mesh_data->num_vertices, mesh_data->num_indices, mesh_data->num_uv };

		uint size = sizeof(ranges) + sizeof(float) * mesh_data->num_vertices * 3 + sizeof(uint) * mesh_data->num_indices + sizeof(float)* mesh_data->num_uv * 2;

		char* data = new char[size]; // Why if the variable is cursor physfs crashes? 
		char* cursor = data;

		// First store ranges
		uint bytes = sizeof(ranges);
		memcpy(cursor, ranges, bytes);

		cursor += bytes;

		//Store vertices
		bytes = sizeof(float) * mesh_data->num_vertices * 3;
		memcpy(cursor, mesh_data->vertices, bytes);

		cursor += bytes;

		//Store indices
		bytes = sizeof(uint) * mesh_data->num_indices;
		memcpy(cursor, mesh_data->indices, bytes);

		cursor += bytes;

		bytes = sizeof(float)* mesh_data->num_uv * 2;
		memcpy(cursor, mesh_data->uv, bytes);

		int i = sizeof(bytes);

		char* final_path = App->fs->SaveFile((char *)data, size, output_file, MESH_FILE);
		
		game_object->mesh->SetPath(final_path);

		RELEASE_ARRAY(data);
	
	}
	else
	{
		RELEASE(game_object);
		game_object = obj;
	}

	for (uint i = 0; i < node->mNumChildren; i++)
	{
		LoadModel(scene, node->mChildren[i], output_file, game_object, component_transform);
	}

	RELEASE(component_transform);
}


Mesh* MeshImporter::LoadFLN(const void* buffer, uint size) {

	Mesh* ret = new Mesh;
	char* cursor = (char*)buffer;

	uint ranges[3];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);

	cursor += bytes;

	ret->num_vertices = ranges[0];
	ret->num_indices = ranges[1];
	ret->num_uv = ranges[2];

	bytes = sizeof(float) * ret->num_vertices * 3;
	ret->vertices = new float[ret->num_vertices * 3];

	memcpy(ret->vertices, cursor, bytes);

	cursor += bytes;

	bytes = sizeof(uint) * ret->num_indices;
	ret->indices = new uint[ret->num_indices];

	memcpy(ret->indices, cursor, bytes);

	cursor += bytes;

	bytes = sizeof(float) * ret->num_uv * 2;
	ret->uv = new float[ret->num_uv * 2];
	memcpy(ret->uv, cursor, bytes);

	ret->felina_path = new char[DEFAULT_BUF_SIZE];

	


	return ret;

}

Mesh* MeshImporter::LoadFLN(const char* path) {

	Mesh* ret = nullptr;

	char* buffer;
	uint size = App->fs->Load(path, &buffer);
	if (size > 0)
	{
		LOG("MATERIAL IMPORTER: Successfully loaded mesh (own format)");
		ret = LoadFLN(buffer, size);
		RELEASE_ARRAY(buffer);
	}
	else
		LOG("MATERIAL IMPORTER: Could not load mesh (own format)");

	return ret;


}

void MeshImporter::CreateFileMeta(Resource* resource, MeshSettings* settings)
{
	JSON_Value* root_value = json_value_init_object();
	JSON_Object* root_object = json_value_get_object(root_value);

	json_object_set_number(root_object, "Time", App->time_management->ReadRealTimeClock());
	json_object_set_number(root_object, "UID", resource->GetUID());

	JSON_Value* mesh_import = json_value_init_object();
	JSON_Object* settings_import = json_value_get_object(mesh_import);

	json_object_set_value(root_object, "Import Settings", mesh_import);

	json_object_set_boolean(settings_import, "Ai PROCCES", settings->process_node);
	json_object_set_boolean(settings_import, "Tangent", settings->CalcTangentSpace);
	json_object_set_boolean(settings_import, "Identical Vertices", settings->JoinIdenticalVertices);
	json_object_set_boolean(settings_import, "Left Handed", settings->MakeLeftHanded);
	json_object_set_boolean(settings_import, "Triangulate", settings->Triangulate);
	json_object_set_boolean(settings_import, "Remove Components", settings->RemoveComponent);
	json_object_set_boolean(settings_import, "Generate Normals", settings->GenNormals);
	json_object_set_boolean(settings_import, "Smooth Normals", settings->GenSmoothNormals);
	json_object_set_boolean(settings_import, "Split Meshes", settings->SplitLargeMeshes);
	json_object_set_boolean(settings_import, "Pre Transform", settings->PreTransformVertices);
	json_object_set_boolean(settings_import, "Limit Bone", settings->LimitBoneWeights);
	json_object_set_boolean(settings_import, "Validate Data", settings->ValidateDataStructure);
	json_object_set_boolean(settings_import, "Remove Redundants", settings->RemoveRedundantMaterials);
	json_object_set_boolean(settings_import, "Sort by Type", settings->SortByPType);
	json_object_set_boolean(settings_import, "Generate Uv", settings->GenUvCoords);
	json_object_set_boolean(settings_import, "Optimize Meshes", settings->OptimizeMeshes);
	json_object_set_boolean(settings_import, "Flip Uvs", settings->FlipUVs);

	char path[DEFAULT_BUF_SIZE];
	strcpy(path, resource->GetExportedFile());
	strcat(path, ".meta");

	uint size = json_serialization_size_pretty(root_value);
	char* buffer = new char[size];

	json_serialize_to_buffer_pretty(root_value, buffer, size);

	App->fs->SaveBufferData(buffer, path, size);

	RELEASE_ARRAY(buffer);
}

void MeshImporter::ReadFileMeta(const char* file, MeshSettings* settings)
{
	char* buffer = nullptr;

	uint size = App->fs->Load(file, &buffer);

	if (size > 0)
	{
		LOG("Can load Mesh meta file: %s",file);

		JSON_Value* root = json_parse_string(buffer);
		JSON_Object* root_object = json_value_get_object(root);

		Resource* resource = App->resource_manager->CreateNewResource(RESOURCE_MESH);
		resource->uid = json_object_get_number(root_object, "UID");
		resource->exported_file = App->gui->file_focus.c_str();

		JSON_Object* import_settings = json_object_get_object(root_object, "Import Settings");

		settings->process_node = (MeshSettings::ProcessNode)json_object_get_boolean(import_settings, "Ai PROCCES");
		settings->CalcTangentSpace= json_object_get_boolean(import_settings, "Tangent");
		settings->JoinIdenticalVertices = json_object_get_boolean(import_settings, "Identical Vertices");
		settings->MakeLeftHanded = json_object_get_boolean(import_settings, "Left Handed");
		settings->Triangulate = json_object_get_boolean(import_settings, "Triangulate");
		settings->RemoveComponent = json_object_get_boolean(import_settings, "Remove Components");
		settings->GenNormals = json_object_get_boolean(import_settings, "Generate Normals");
		settings->GenSmoothNormals = json_object_get_boolean(import_settings, "Smooth Normals");
		settings->SplitLargeMeshes = json_object_get_boolean(import_settings, "Split Meshes");
		settings->PreTransformVertices = json_object_get_boolean(import_settings, "Pre Transform");
		settings->LimitBoneWeights = json_object_get_boolean(import_settings, "Limit Bone");
		settings->ValidateDataStructure = json_object_get_boolean(import_settings, "Validate Data");
		settings->RemoveRedundantMaterials = json_object_get_boolean(import_settings, "Remove Redundants");
		settings->SortByPType = json_object_get_boolean(import_settings, "Sort by Type");
		settings->GenUvCoords = json_object_get_boolean(import_settings, "Generate Uv");
		settings->OptimizeMeshes = json_object_get_boolean(import_settings, "Optimize Meshes");
		settings->FlipUVs = json_object_get_boolean(import_settings, "Flip Uvs");

		json_value_free(root);
	}
	else
		LOG("Error reading mesh meta file: %s", file);

	RELEASE_ARRAY(buffer);
}

void MeshImporter::ShowMeshImport()
{

	static int current_procces = mesh_settings->process_node;
	const char* process_elements[] = { " TargetRealtime_MaxQuality" , "TargetRealtime_Quality", "TargetRealtime_Fast", "ConvertToLeftHanded " };


	ImGui::Text("Procces:");
	ImGui::SameLine();
	if (ImGui::Combo("###procces2", &current_procces, process_elements, ((int)(sizeof(process_elements) / sizeof(*process_elements)))))
	{
		mesh_settings->process_node = (MeshSettings::ProcessNode)current_procces;
		RefreshMeshOptions();
	}

	ImGui::Checkbox("Calculate Tangent Space:", &mesh_settings->CalcTangentSpace);
	ImGui::Checkbox("Join Identical Vertices:", &mesh_settings->JoinIdenticalVertices);
	ImGui::Checkbox("Make Left Handed:", &mesh_settings->MakeLeftHanded);
	ImGui::Checkbox("Triangulate Vertices:", &mesh_settings->Triangulate);
	ImGui::Checkbox("Remove Components:", &mesh_settings->RemoveComponent);
	ImGui::Checkbox("Generate Normals:", &mesh_settings->GenNormals);
	ImGui::Checkbox("Generate Smooth Normals:", &mesh_settings->GenSmoothNormals);
	ImGui::Checkbox("Split Large Meshes:", &mesh_settings->SplitLargeMeshes);
	ImGui::Checkbox("Pre Transform Vertices:", &mesh_settings->PreTransformVertices);
	ImGui::Checkbox("Limit Bone Weights:", &mesh_settings->LimitBoneWeights);
	ImGui::Checkbox("Validate Data Structure:", &mesh_settings->ValidateDataStructure);
	ImGui::Checkbox("Remove Redundant Materials:", &mesh_settings->RemoveRedundantMaterials);
	ImGui::Checkbox("Sort by Type:", &mesh_settings->SortByPType);
	ImGui::Checkbox("Generate Uvs Coords:", &mesh_settings->GenUvCoords);
	ImGui::Checkbox("Optimize Meshes:", &mesh_settings->OptimizeMeshes);
	ImGui::Checkbox("Flip UVs:", &mesh_settings->FlipUVs);

	if (ImGui::Button("Apply ###importmesh", { 70,50 }))
	{
		uint uid = App->resource_manager->Find(App->gui->file_focus.c_str());
		Resource* resource = App->resource_manager->Get(uid);

		CreateFileMeta(resource, mesh_settings);
		App->gui->file_focus.clear();
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel ###savemesh", { 70,50 }))
	{
		App->gui->file_focus.clear();
	}

}

void MeshImporter::RefreshMeshOptions()
{
	switch (mesh_settings->process_node)
	{
	case MeshSettings::ProcessNode::TargetRealtime_MaxQuality:
		mesh_settings->CalcTangentSpace = true;
		mesh_settings->JoinIdenticalVertices = true;
		mesh_settings->MakeLeftHanded = false;
		mesh_settings->Triangulate = true;
		mesh_settings->RemoveComponent = false;
		mesh_settings->GenNormals = false;
		mesh_settings->GenSmoothNormals = true;
		mesh_settings->SplitLargeMeshes = true;
		mesh_settings->PreTransformVertices = false;
		mesh_settings->LimitBoneWeights = true;
		mesh_settings->ValidateDataStructure = true;
		mesh_settings->RemoveRedundantMaterials = true;
		mesh_settings->SortByPType = true;
		mesh_settings->GenUvCoords = true;
		mesh_settings->OptimizeMeshes = true;
		mesh_settings->FlipUVs = false;
		break;
	case MeshSettings::ProcessNode::TargetRealtime_Quality:
		mesh_settings->CalcTangentSpace = true;
		mesh_settings->JoinIdenticalVertices = true;
		mesh_settings->MakeLeftHanded = false;
		mesh_settings->Triangulate = true;
		mesh_settings->RemoveComponent = false;
		mesh_settings->GenNormals = false;
		mesh_settings->GenSmoothNormals = true;
		mesh_settings->SplitLargeMeshes = true;
		mesh_settings->PreTransformVertices = false;
		mesh_settings->LimitBoneWeights = true;
		mesh_settings->ValidateDataStructure = false;
		mesh_settings->RemoveRedundantMaterials = true;
		mesh_settings->SortByPType = true;
		mesh_settings->GenUvCoords = true;
		mesh_settings->OptimizeMeshes = false;
		mesh_settings->FlipUVs = false;
		break;
	case MeshSettings::ProcessNode::TargetRealtime_Fast:
		mesh_settings->CalcTangentSpace = true;
		mesh_settings->JoinIdenticalVertices = true;
		mesh_settings->MakeLeftHanded = false;
		mesh_settings->Triangulate = true;
		mesh_settings->RemoveComponent = false;
		mesh_settings->GenNormals = true;
		mesh_settings->GenSmoothNormals = false;
		mesh_settings->SplitLargeMeshes = false;
		mesh_settings->PreTransformVertices = false;
		mesh_settings->LimitBoneWeights = false;
		mesh_settings->ValidateDataStructure = false;
		mesh_settings->RemoveRedundantMaterials = false;
		mesh_settings->SortByPType = true;
		mesh_settings->GenUvCoords = true;
		mesh_settings->OptimizeMeshes = false;
		mesh_settings->FlipUVs = false;
		break;
	case MeshSettings::ProcessNode::ConvertToLeftHanded:
		mesh_settings->CalcTangentSpace = false;
		mesh_settings->JoinIdenticalVertices = false;
		mesh_settings->MakeLeftHanded = true;
		mesh_settings->Triangulate = false;
		mesh_settings->RemoveComponent = false;
		mesh_settings->GenNormals = false;
		mesh_settings->GenSmoothNormals = false;
		mesh_settings->SplitLargeMeshes = false;
		mesh_settings->PreTransformVertices = false;
		mesh_settings->LimitBoneWeights = false;
		mesh_settings->ValidateDataStructure = false;
		mesh_settings->RemoveRedundantMaterials = false;
		mesh_settings->SortByPType = false;
		mesh_settings->GenUvCoords = false;
		mesh_settings->OptimizeMeshes = false;
		mesh_settings->FlipUVs = true;
		break;
	default:
		break;
	}
}

uint MeshImporter::SetPostProccesConfiguration(const MeshSettings* import_settings)
{
	uint flags = 0;

	switch (import_settings->process_node)
	{
	case MeshSettings::ProcessNode::TargetRealtime_MaxQuality:
		flags |= aiProcessPreset_TargetRealtime_MaxQuality;
		break;
	case MeshSettings::ProcessNode::TargetRealtime_Quality:
		flags |= aiProcessPreset_TargetRealtime_Quality;
		break;
	case MeshSettings::ProcessNode::TargetRealtime_Fast:
		flags |= aiProcessPreset_TargetRealtime_Fast;
		break;
	case MeshSettings::ProcessNode::ConvertToLeftHanded:
		flags |= aiProcess_ConvertToLeftHanded;
		break;
	}

	if (import_settings->CalcTangentSpace)
		flags |= aiPostProcessSteps::aiProcess_CalcTangentSpace;
	if (import_settings->JoinIdenticalVertices)
		flags |= aiPostProcessSteps::aiProcess_JoinIdenticalVertices;
	if (import_settings->MakeLeftHanded)
		flags |= aiPostProcessSteps::aiProcess_MakeLeftHanded;
	if (import_settings->Triangulate)
		flags |= aiPostProcessSteps::aiProcess_Triangulate;
	if (import_settings->RemoveComponent)
		flags |= aiPostProcessSteps::aiProcess_RemoveComponent;
	if (import_settings->GenNormals)
		flags |= aiPostProcessSteps::aiProcess_GenNormals;
	if (import_settings->GenSmoothNormals)
		flags |= aiPostProcessSteps::aiProcess_GenSmoothNormals;
	if (import_settings->SplitLargeMeshes)
		flags |= aiPostProcessSteps::aiProcess_SplitLargeMeshes;
	if (import_settings->PreTransformVertices)
		flags |= aiPostProcessSteps::aiProcess_PreTransformVertices;
	if (import_settings->LimitBoneWeights)
		flags |= aiPostProcessSteps::aiProcess_LimitBoneWeights;
	if (import_settings->ValidateDataStructure)
		flags |= aiPostProcessSteps::aiProcess_ValidateDataStructure;
	if (import_settings->RemoveRedundantMaterials)
		flags |= aiPostProcessSteps::aiProcess_RemoveRedundantMaterials;
	if (import_settings->SortByPType)
		flags |= aiPostProcessSteps::aiProcess_SortByPType;
	if (import_settings->GenUvCoords)
		flags |= aiPostProcessSteps::aiProcess_GenUVCoords;
	if (import_settings->OptimizeMeshes)
		flags |= aiPostProcessSteps::aiProcess_OptimizeMeshes;
	if (import_settings->FlipUVs)
		flags |= aiPostProcessSteps::aiProcess_FlipUVs;

	return flags;
}