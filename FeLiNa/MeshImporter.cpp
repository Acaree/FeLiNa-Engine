#include "MeshImporter.h"

#include "Glew/include/glew.h" 
#include "SDL/include/SDL_opengl.h"

#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"

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

		//TO REVISE: I think this can delete
		//childrens_go->SetParent(App->scene->root_object);
		//App->scene->root_object->AddChildren(childrens_go);

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
		//Create a game object components
		//ComponentTransform* tr = component_trans;

		//tr->SetParent(game_object);

		//game_object->SetComponent(component_transform);

		ComponentMesh* component_mesh = (ComponentMesh*)game_object->AddComponent(Component_Mesh);
		ComponentTexture* component_texture = (ComponentTexture*)game_object->AddComponent(Component_Material);

		ComponentTransform* transform = (ComponentTransform*)game_object->AddComponent(Component_Transform);
		transform->SetPosition(component_transform->GetPosition());
		transform->SetRotation(component_transform->GetRotation());
		transform->SetScale(component_transform->GetScale());
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
		/*game_object->SetComponent(tr);
		game_object->SetComponent(component_mesh);
		game_object->SetComponent(component_texture);*/

		//game_object->SetParent(obj);
		obj->AddChildren(game_object);
		game_object->RecalculateBoundingBox();

		/*******************************************************************/

		//put in a new function savefln
		uint ranges[2] = { mesh_data->num_indices, mesh_data->num_vertices};
		uint size = sizeof(ranges) + sizeof(uint) * mesh_data->num_indices + sizeof(float) * mesh_data->num_vertices * 3;

		char* data = new char[size]; // Allocate
		char* cursor = data;
		uint bytes = sizeof(ranges); // First store ranges
		memcpy(cursor, ranges, bytes);

		float* vertices_ = new float[mesh_data->num_vertices * 3];

		//memcpy(vertices_, mesh_data->vertices, sizeof(float) * mesh_data->num_vertices * 3);
		
		cursor += bytes;
		bytes = sizeof(float) * mesh_data->num_vertices * 3;

		//TO TEST (delete to assignment)
		std::vector<float>  test_vector;
		test_vector.assign(mesh_data->num_vertices * 3, 0.0f);
		memcpy(test_vector.data(), mesh_data->vertices, sizeof(float)*mesh_data->num_vertices * 3);

		memcpy(cursor, mesh_data->vertices, bytes);

		cursor += bytes; // Store indices
		bytes = sizeof(uint) * mesh_data->num_indices;
		memcpy(cursor, mesh_data->indices, bytes);

		//TO TEST (delete to assignment)
		std::vector<uint>  test_vector2;
		test_vector2.assign(mesh_data->num_indices, 0.0f);
		memcpy(test_vector2.data(), mesh_data->indices, sizeof(uint)*mesh_data->num_indices);
		

		/***************************************************************************************/

		
		output_file = node->mName.data;

		
		if (App->fs->SaveTexture((char *)data, size, output_file))
		{
			LOG("SCENE IMPORTER: Successfully saved mesh to own format");
		}

		//MEM LEAK

		//RELEASE_ARRAY(data);
		//RELEASE_ARRAY(vertices_);


	}
	else
	{
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

	glGenBuffers(1, (GLuint*) &(mesh_data->id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_data->id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh_data->num_indices, mesh_data->indices, GL_STATIC_DRAW);

	glGenBuffers(1, (GLuint*) &(mesh_data->id_uv));
	glBindBuffer(GL_ARRAY_BUFFER, mesh_data->id_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * mesh_data->num_uv, mesh_data->uv, GL_STATIC_DRAW);

}

Mesh* MeshImporter::LoadFLN(const void* buffer, uint size) {

	Mesh* ret = new Mesh;
	char* cursor = (char*)buffer;
	// amount of indices / vertices / colors / normals / texture_coords
	uint ranges[2];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	
	ret->num_indices = ranges[0];
	ret->num_vertices = ranges[1];
	// Load indices
	cursor += bytes;
	bytes = sizeof(float) * ret->num_vertices * 3;
	ret->vertices = new float[ret->num_vertices * 3];
	memcpy(ret->vertices, cursor, bytes);

	//TO TEST (delete to assignment)
	std::vector<float>  test_vector;
	test_vector.assign(ret->num_vertices * 3, 0.0f);
	memcpy(test_vector.data(), ret->vertices, sizeof(float)*ret->num_vertices * 3);

	
	cursor += bytes;
	bytes = sizeof(uint) * ret->num_indices;
	ret->indices = new uint[ret->num_indices];
	memcpy(ret->indices, cursor, bytes);

	//TO TEST (delete to assignment)
	std::vector<uint>  test_vector2;
	test_vector2.assign(ret->num_indices, 0.0f);
	memcpy(test_vector2.data(), ret->indices, sizeof(uint)*ret->num_indices);
	

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

