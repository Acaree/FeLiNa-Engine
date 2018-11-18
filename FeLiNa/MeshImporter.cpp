#include "MeshImporter.h"
#include "PhysFS/physfs.h"
#include "Glew/include/glew.h" 
#include "SDL/include/SDL_opengl.h"
#include "ModuleGui.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include "ModuleTimeManagement.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ModuleResourceManager.h"
#include "ResourceMesh.h"
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
		ret = Import(buffer, size, output_file,import_settings, file_path);
		RELEASE_ARRAY(buffer);
	}	
	else
		LOG("MATERIAL IMPORTER: Could not load texture ");

	
	return ret;
}

bool MeshImporter::Import(const void* buffer, uint size, std::string& output_file, const MeshSettings* import_settings, const char* file_path)
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

		LoadModel(scene, rootNode, childrens_go, trans);

		//Serialization the imported file
		App->serialization_scene->save_name_scene = childrens_go->GetName();
		App->serialization_scene->SaveScene(childrens_go,file_path);

		aiReleaseImport(scene);

		RELEASE(childrens_go);

		ret = true;
	}

	return ret;
}

void MeshImporter::LoadModel(const aiScene* scene, aiNode* node, GameObject* parent, ComponentTransform* transform)
{

	GameObject* go = parent;

	std::string name = node->mName.C_Str();
	static int invalid_position = std::string::npos;

	//Street has a node with size 0 :/.
	if (name.length() == 0)
		name = "No Name";

	//best way? Invalid nodes of assimp only have transformations
	static const char* invalid_node_names[5] = { "$_PreRotation", "$_Rotation", "$_PostRotation",
		"$_Scaling", "$_Translation" };

	bool invalid_node = false;
	if (name.find(invalid_node_names[0]) != invalid_position || name.find(invalid_node_names[1]) != invalid_position || name.find(invalid_node_names[2]) != invalid_position
		|| name.find(invalid_node_names[3]) != invalid_position || name.find(invalid_node_names[4]) != invalid_position)
		invalid_node = true;

	aiVector3D position;
	aiVector3D scale;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scale, rotation, position);

	math::float3 new_position = { position.x, position.y, position.z };
	math::Quat new_rotation = { rotation.x, rotation.y, rotation.z, rotation.w };
	math::float3 new_scale = { scale.x, scale.y, scale.z };

	
	transform->SumPosition(new_position);
	transform->SumRotation(new_rotation.ToEulerXYZ());
	transform->SumScale(new_scale);

	if (!invalid_node && node != scene->mRootNode)
	{
		//if are a valid node we set the transformation of all last invalid node.
		go = new GameObject(parent);
		go->SetName(name.c_str());
		go->AddComponent(Component_Transform);
		go->transform->SetPosition(transform->GetPosition());
		go->transform->SetRotation(transform->GetRotation());
		go->transform->SetScale(transform->GetScale());

		//Set default transformation because if node are valid because, because the new game object has all transformations.
		transform->SetPosition({ 0,0,0 });
		transform->SetRotation({ 0,0,0, });
		transform->SetScale({ 1,1,1 });
	
	}

	if (node->mNumMeshes > 0)
	{
		aiMesh* new_mesh = scene->mMeshes[node->mMeshes[0]];

		invalid_node = false;

		//Check faces are a problem with street-.-

		for (uint i = 0; i < new_mesh->mNumFaces; ++i)
		{
			if (new_mesh->mFaces[i].mNumIndices != 3)
			{
				invalid_node = true;
				LOG("INVALID MESH: %s her face have number of indices < 3",name);
			}
		}

		if (!invalid_node)
		{
			bool is_loaded = false;

			go->AddComponent(Component_Mesh);


			if (meshes_map.find(new_mesh) != meshes_map.end())
			{
				go->mesh->SetUID(meshes_map.find(new_mesh)->second);
				is_loaded = true;
			}
			else
				meshes_map[new_mesh] = go->mesh->GetUID();

			//Create here all information:

			float* vertices = nullptr;
			uint num_vertices = 0;
			uint id_vertices = 0;

			uint* indices = nullptr;
			uint num_indices = 0;
			uint id_indices = 0;

			float* uv = nullptr;
			uint num_uv = 0;
			uint id_uv = 0;

			aiMaterial* material = scene->mMaterials[new_mesh->mMaterialIndex];

			if (material != nullptr)
			{
				aiString name;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &name);

				std::string tmp_name = name.C_Str();
				std::string output_file;
				tmp_name.erase(0, tmp_name.find_last_of("\\") + 1);

				if (tmp_name.size() > 0)
				{
					if (App->fs->RecursiveFindFileExistInAssets("Assets", tmp_name.c_str(), output_file))
					{
						uint uid = App->resource_manager->Find(output_file.c_str());

						if (uid == 0)
							uid = App->resource_manager->ImportFile(output_file.c_str());

						go->AddComponent(Component_Material);
						go->material->SetUID(uid);

					}
				}

			}

			if (!is_loaded)
			{
				//Load Vertices
				num_vertices = new_mesh->mNumVertices;
				vertices = new float[num_vertices * 3];
				memcpy(vertices, new_mesh->mVertices, sizeof(float)*num_vertices * 3);
				LOG("New mesh with %d vertices", num_vertices);

				if (new_mesh->HasFaces())
				{
					num_indices = new_mesh->mNumFaces * 3;
					indices = new uint[num_indices];

					for (uint num_faces = 0; num_faces < new_mesh->mNumFaces; ++num_faces)
					{
						memcpy(&indices[num_faces * 3], new_mesh->mFaces[num_faces].mIndices, 3 * sizeof(uint));
					}

				}

				if (new_mesh->HasTextureCoords(0))
				{
					num_uv = new_mesh->mNumVertices;
					uv = new float[num_uv * 2];

					for (uint coords = 0; coords < new_mesh->mNumVertices; ++coords)
					{
						memcpy(&uv[coords * 2], &new_mesh->mTextureCoords[0][coords].x, sizeof(float));
						memcpy(&uv[(coords * 2) + 1], &new_mesh->mTextureCoords[0][coords].y, sizeof(float));
					}


				}

				uint ranges[3] = { num_vertices,num_indices, num_uv };

				uint size = sizeof(ranges) + sizeof(float) * num_vertices * 3 + sizeof(uint) * num_indices + sizeof(float)* num_uv * 2;

				char* data = new char[size]; // Why if the variable is cursor physfs crashes? 
				char* cursor = data;

				// First store ranges
				uint bytes = sizeof(ranges);
				memcpy(cursor, ranges, bytes);

				cursor += bytes;

				//Store vertices
				bytes = sizeof(float) * num_vertices * 3;
				memcpy(cursor, vertices, bytes);

				cursor += bytes;

				//Store indices
				bytes = sizeof(uint) * num_indices;
				memcpy(cursor, indices, bytes);

				cursor += bytes;

				bytes = sizeof(float)* num_uv * 2;
				memcpy(cursor, uv, bytes);

				int i = sizeof(bytes);

				std::string output_file = std::to_string(go->mesh->GetUID());
				char* final_path = App->fs->SaveFile((char *)data, size, output_file, MESH_FILE);

				RELEASE_ARRAY(data);



				RELEASE_ARRAY(vertices);
				RELEASE_ARRAY(indices);
				RELEASE_ARRAY(uv);
			}
		}
	}

	for (uint i = 0; i < node->mNumChildren; ++i)
	{
		LoadModel(scene, node->mChildren[i], go, transform);
	}
	
}


bool MeshImporter::LoadFLN(const void* buffer, uint size, ResourceMesh* ret) {

	if (buffer == nullptr || ret == nullptr)
		return false;


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


	return true;

}

bool MeshImporter::LoadFLN(const char* path, ResourceMesh* mesh) {

	bool ret = false;

	char* buffer;
	uint size = App->fs->Load(path, &buffer);
	if (size > 0)
	{
		LOG("MATERIAL IMPORTER: Successfully loaded mesh (own format)");
		ret = LoadFLN(buffer, size,mesh);
		RELEASE_ARRAY(buffer);
	}
	else
		LOG("MATERIAL IMPORTER: Could not load mesh (own format)");

	return ret;


}

void MeshImporter::SetMeshSettingsFromMeta(const char* file, MeshSettings* settings)
{
	if (file != nullptr && settings != nullptr)
	{
		char* buffer;
		uint size = App->fs->Load(file, &buffer);

		if (size > 0)
		{
			LOG("Can load Mesh meta file: %s", file);

			JSON_Value* root = json_parse_string(buffer);
			JSON_Object* root_object = json_value_get_object(root);

			json_object_set_number(root_object, "Time", 1);

			JSON_Object* settings_import = json_object_get_object(root_object, "Import Settings");

			json_object_set_number(settings_import, "Ai PROCCES", settings->process_node);
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


			uint size = json_serialization_size_pretty(root);
			char* new_buffer = new char[size];


			json_serialize_to_buffer_pretty(root, new_buffer, size);

			App->fs->SaveBufferData(new_buffer, file, size);

			//RELEASE_ARRAY(new_buffer);
			json_value_free(root);
		}
		else
			LOG("Error reading mesh meta file: %s", file);

		RELEASE_ARRAY(buffer);
	}

}

void MeshImporter::CreateFileMeta(std::list<Resource*> resources, MeshSettings* settings)
{
	if (resources.size() > 0)
	{
		JSON_Value* root_value = json_value_init_object();
		JSON_Object* root_object = json_value_get_object(root_value);

		JSON_Value* array_value = json_value_init_array();
		JSON_Array* array_uids = json_value_get_array(array_value);

		ResourceMesh* resource = (ResourceMesh*)resources.front();
		json_object_set_number(root_object, "Time", App->fs->GetLastModificationTime(resource->file.c_str()));

		for (std::list<Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
			json_array_append_number(array_uids, (*it)->GetUID());

		json_object_set_value(root_object, "UID MESHES", array_value);



		JSON_Value* mesh_import = json_value_init_object();
		JSON_Object* settings_import = json_value_get_object(mesh_import);

		json_object_set_value(root_object, "Import Settings", mesh_import);

		json_object_set_number(settings_import, "Ai PROCCES", settings->process_node);
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
		strcpy(path, resource->file.c_str());
		strcat(path, ".meta");

		uint size = json_serialization_size_pretty(root_value);
		char* buffer = new char[size];

		json_serialize_to_buffer_pretty(root_value, buffer, size);

		App->fs->SaveBufferData(buffer, path, size);


		RELEASE_ARRAY(buffer);
	
	}
}
//To REVISE: we can delete this function and use Get Mesh Settings from meta?
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

		std::string tmp = file;
		tmp = tmp.substr(0, tmp.find_last_of("."));

		resource->exported_file = tmp.c_str();

		JSON_Object* import_settings = json_object_get_object(root_object, "Import Settings");

		settings->process_node = (MeshSettings::ProcessNode)(int)json_object_get_number(import_settings, "Ai PROCCES");
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

void MeshImporter::GetMeshSettingsFromMeta(const char* file, MeshSettings* settings)
{
	if (file != nullptr && settings != nullptr)
	{
		char* buffer;
		uint size = App->fs->Load(file, &buffer);

		if(size > 0)
		{
			JSON_Value* root_value = json_parse_string(buffer);
			JSON_Object* root_object = json_value_get_object(root_value);

			JSON_Object* import_settings = json_object_get_object(root_object, "Import Settings");

			settings->process_node = (MeshSettings::ProcessNode)(int)json_object_get_number(import_settings, "Ai PROCCES");
			settings->CalcTangentSpace = json_object_get_boolean(import_settings, "Tangent");
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

			json_value_free(root_value);
		}
		RELEASE_ARRAY(buffer);
	}
}


void MeshImporter::ShowMeshImport()
{

	static int current_procces = mesh_settings->process_node;
	const char* process_elements[] = { " TargetRealtime_MaxQuality" , "TargetRealtime_Quality", "TargetRealtime_Fast", "ConvertToLeftHanded", "Customize" };

	bool is_customize = false;

	ImGui::Text(" Import options for: %s ###importmesh", App->gui->file_focus.c_str());

	

	ImGui::Text("Procces:");
	ImGui::SameLine();

	if (ImGui::Combo("###procces2", &current_procces, process_elements, ((int)(sizeof(process_elements) / sizeof(*process_elements)))))
	{
		mesh_settings->process_node = (MeshSettings::ProcessNode)current_procces;
		RefreshMeshOptions();
	}

	if (current_procces == MeshSettings::ProcessNode::Customized)
	{
		is_customize = true;
	}

	if (ImGui::Checkbox("Calculate Tangent Space:", &mesh_settings->CalcTangentSpace))
	{
		if (!is_customize)
			mesh_settings->CalcTangentSpace = !mesh_settings->CalcTangentSpace;
	}

	if (ImGui::Checkbox("Join Identical Vertices:", &mesh_settings->JoinIdenticalVertices))
	{
		if (!is_customize)
			mesh_settings->JoinIdenticalVertices = !mesh_settings->JoinIdenticalVertices;
	}

	if (ImGui::Checkbox("Make Left Handed:", &mesh_settings->MakeLeftHanded))
	{
		if (!is_customize)
			mesh_settings->MakeLeftHanded = !mesh_settings->MakeLeftHanded;
	}

	if (ImGui::Checkbox("Triangulate Vertices:", &mesh_settings->Triangulate))
	{
		if (!is_customize)
			mesh_settings->Triangulate = !mesh_settings->Triangulate;
	}

	if (ImGui::Checkbox("Remove Components:", &mesh_settings->RemoveComponent))
	{
		if (!is_customize)
			mesh_settings->RemoveComponent = !mesh_settings->RemoveComponent;
	}

	if (ImGui::Checkbox("Generate Normals:", &mesh_settings->GenNormals))
	{
		if (!is_customize)
			mesh_settings->GenNormals = !mesh_settings->GenNormals;
	}

	if (ImGui::Checkbox("Generate Smooth Normals:", &mesh_settings->GenSmoothNormals))
	{
		if (!is_customize)
			mesh_settings->GenSmoothNormals = !mesh_settings->GenSmoothNormals;
	}

	if (ImGui::Checkbox("Split Large Meshes:", &mesh_settings->SplitLargeMeshes))
	{
		if (!is_customize)
			mesh_settings->SplitLargeMeshes = !mesh_settings->SplitLargeMeshes;
	}

	if (ImGui::Checkbox("Pre Transform Vertices:", &mesh_settings->PreTransformVertices))
	{
		if (!is_customize)
			mesh_settings->PreTransformVertices = !mesh_settings->PreTransformVertices;
	}

	if (ImGui::Checkbox("Limit Bone Weights:", &mesh_settings->LimitBoneWeights))
	{
		if (!is_customize)
			mesh_settings->LimitBoneWeights = !mesh_settings->LimitBoneWeights;
	}

	if (ImGui::Checkbox("Validate Data Structure:", &mesh_settings->ValidateDataStructure))
	{
		if (!is_customize)
			mesh_settings->ValidateDataStructure = !mesh_settings->ValidateDataStructure;
	}

	if (ImGui::Checkbox("Remove Redundant Materials:", &mesh_settings->RemoveRedundantMaterials))
	{
		if (!is_customize)
			mesh_settings->RemoveRedundantMaterials = !mesh_settings->RemoveRedundantMaterials;
	}

	if (ImGui::Checkbox("Sort by Type:", &mesh_settings->SortByPType))
	{
		if (!is_customize)
			mesh_settings->SortByPType = !mesh_settings->SortByPType;
	}

	if (ImGui::Checkbox("Generate Uvs Coords:", &mesh_settings->GenUvCoords))
	{
		if (!is_customize)
			mesh_settings->GenUvCoords = !mesh_settings->GenUvCoords;
	}

	if (ImGui::Checkbox("Optimize Meshes:", &mesh_settings->OptimizeMeshes))
	{
		if (!is_customize)
			mesh_settings->OptimizeMeshes = !mesh_settings->OptimizeMeshes;
	}

	if (ImGui::Checkbox("Flip UVs:", &mesh_settings->FlipUVs))
	{
		if (!is_customize)
			mesh_settings->FlipUVs =  !mesh_settings->FlipUVs
;
	}

	if (ImGui::Button("Apply ###importermesh", { 70,50 }))
	{
		std::string tmp = App->gui->file_focus;
		tmp += ".meta";

		SetMeshSettingsFromMeta(tmp.c_str(),mesh_settings);
	
		App->fs->refresh_now = true;

		App->gui->file_focus.clear();
		App->gui->show_import_settings = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel ###savemesh", { 70,50 }))
	{
		App->gui->show_import_settings = false;
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

	case MeshSettings::ProcessNode::Customized:
		
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

		break;
	}

	return flags;
}