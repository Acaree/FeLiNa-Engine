#include "MeshImporter.h"

#include "Glew/include/glew.h" 
#include "SDL/include/SDL_opengl.h"

#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"

#include "MaterialImporter.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"


#pragma comment (lib,"Assimp/libx86/assimp.lib")

#include "mmgr/mmgr.h"
MeshImporter::MeshImporter()
{
	
}


MeshImporter::~MeshImporter()
{

}
//const char* importFileName, const char* importPath, std::string& outputFileName
bool MeshImporter::Import(const char* file_name, const char* file_path, std::string& output_file)
{
	bool ret = false;

	char* buffer;

	//Check if file is !null
	if (file_path == nullptr)
		return ret;

	char importer_path[DEFAULT_BUF_SIZE]; //importFilePath

	strcpy_s(importer_path, strlen(file_path) + 1, file_path);

	if (file_name != nullptr)
		strcat_s(importer_path, strlen(importer_path) + strlen(file_name) + 1, file_name);

	output_file = App->fs->GetNameFile(importer_path);

	uint size = App->fs->Load(importer_path, &buffer);

	if (size > 0)
	{
		LOG("MATERIAL IMPORTER: Successfully");

		ret = Import(buffer, size, output_file);
		RELEASE_ARRAY(buffer);
	}
	else
		LOG("MATERIAL IMPORTER: Could not load texture ");

	return ret;
}

//const void* buffer, uint size, std::string& outputFileName
bool MeshImporter::Import(const void* buffer, uint size, std::string& output_file)
{
	bool ret = false;

	LOG("Inicialization load data model");


	const aiScene* scene = aiImportFileFromMemory((const char*)buffer, size, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	if (scene != nullptr) {

		//Create a new Game Object and set the name of file.
		GameObject* childrens_go = new GameObject(App->scene->root_object);
		childrens_go->SetName((char*)output_file.data());

		aiNode* rootNode = scene->mRootNode;

		ComponentTransform* trans = (ComponentTransform*)childrens_go->AddComponent(Component_Transform);

		LoadModel(scene, rootNode, output_file, childrens_go, trans);

		App->serialization_scene->save_name_scene = "test_auto_save";
		App->serialization_scene->SaveScene();

		aiReleaseImport(scene);

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

		ComponentTransform* transform = (ComponentTransform*)game_object->AddComponent(Component_Transform);
		transform->SetPosition(component_transform->GetPosition());
		transform->SetRotation(component_transform->GetRotation());
		transform->SetScale(component_transform->GetScale());


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

			sprintf_s(add, DEFAULT_BUF_SIZE, "%s/%s/", add, path_c);

 			Texture* tex = App->importer_material->Import((const char*)file_name_c,(const char*)add, texture_generated);

			//Texture* tex = App->importer_material->Import("Baker_house.png", "Assets/Textures/", texture_generated);

			
			
			component_texture->SetTexture(tex);
			
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


			//Add the mesh component
			GenerateBufferData(mesh_data);

			//TO REVISE call here to module renderer?

			App->renderer3D->AddDataMesh(component_mesh);

			//component_mesh = App->renderer3D->CreateComponentMesh();

		}

		//CREATE AABB
		game_object->AddBoundingBox(mesh_data);
		

		obj->AddChildren(game_object);
		game_object->RecalculateBoundingBox();

		/*******************************************************************/

		//put in a new function savefln



		uint ranges[3] = { mesh_data->num_vertices, mesh_data->num_indices, mesh_data->num_uv };

		uint size = sizeof(ranges) + sizeof(float) * mesh_data->num_vertices * 3 + sizeof(uint) * mesh_data->num_indices + sizeof(float)* mesh_data->num_uv * 2;

		char* data = new char[size]; // Allocate
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



		output_file = node->mName.data;


		char* final_path = App->fs->SaveFile((char *)cursor, size, output_file, MESH_FILE);
		


		game_object->mesh->SetPath(final_path);


	}
	else
	{
		RELEASE(game_object);
		game_object = obj;
	}

	for (uint i = 0; i < node->mNumChildren; ++i)
	{
		LoadModel(scene, node->mChildren[i], output_file, game_object, component_transform);
	}

}

void MeshImporter::GenerateBufferData(Mesh* mesh_data)
{
	glGenBuffers(1, (GLuint*) &(mesh_data->id_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, mesh_data->id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh_data->num_vertices, mesh_data->vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, (GLuint*) &(mesh_data->id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_data->id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh_data->num_indices, mesh_data->indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, (GLuint*) &(mesh_data->id_uv));
	glBindBuffer(GL_ARRAY_BUFFER, mesh_data->id_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * mesh_data->num_uv, mesh_data->uv, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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

	GenerateBufferData(ret);


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

