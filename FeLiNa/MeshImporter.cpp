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

		aiNode* rootNode = scene->mRootNode;

		LoadModel(scene, rootNode, output_file);

		//TO REVISE: I think this can delete
		//childrens_go->SetParent(App->scene->root_object);
		//App->scene->root_object->AddChildren(childrens_go);

		aiReleaseImport(scene);

	}

	return ret;
}

void MeshImporter::LoadModel(const aiScene* scene, aiNode* node, std::string& output_file)
{

	if (node->mNumMeshes > 0)
	{

		Mesh mesh_data;

		for (int num_meshes = 0; num_meshes < node->mNumMeshes; ++num_meshes)
		{
			aiMesh* new_mesh = scene->mMeshes[node->mMeshes[num_meshes]];

			//Load Vertices
			mesh_data.num_vertices = new_mesh->mNumVertices;
			mesh_data.vertices = new float[mesh_data.num_vertices * 3];
			memcpy(mesh_data.vertices, new_mesh->mVertices, sizeof(float)*mesh_data.num_vertices * 3);
			LOG("New mesh with %d vertices", mesh_data.num_vertices);

			//Load Mesh
			if (new_mesh->HasFaces())
			{
				mesh_data.num_indices = new_mesh->mNumFaces * 3;
				mesh_data.indices = new uint[mesh_data.num_indices];

				for (uint num_faces = 0; num_faces < new_mesh->mNumFaces; ++num_faces)
				{
					if (new_mesh->mFaces[num_faces].mNumIndices != 3)
					{
						LOG("Geometry face %i whit %i faces", num_faces, new_mesh->mFaces[num_faces].mNumIndices);
					}
					else
						memcpy(&mesh_data.indices[num_faces * 3], new_mesh->mFaces[num_faces].mIndices, 3 * sizeof(uint));

				}


			}

			//Texture
			aiMaterial* material = scene->mMaterials[new_mesh->mMaterialIndex];
			if (new_mesh->HasTextureCoords(0))
			{
				mesh_data.num_uv = new_mesh->mNumVertices;
				mesh_data.uv = new float[mesh_data.num_uv * 2];

				for (uint coords = 0; coords < new_mesh->mNumVertices; ++coords)
				{
					memcpy(&mesh_data.uv[coords * 2], &new_mesh->mTextureCoords[0][coords].x, sizeof(float));
					memcpy(&mesh_data.uv[(coords * 2) + 1], &new_mesh->mTextureCoords[0][coords].y, sizeof(float));
				}


			}


			//Add the mesh component
			GenerateBufferData(&mesh_data);

			//TO REVISE call here to module renderer?

			//component_mesh = App->renderer3D->CreateComponentMesh();

		}

		//CREATE AABB

		/*game_object->SetComponent(tr);
		game_object->SetComponent(component_mesh);
		game_object->SetComponent(component_texture);*/

		//game_object->SetParent(obj);

		/*******************************************************************/


		
		//put in a new function savefln
		uint ranges[3] = { mesh_data.num_vertices, mesh_data.num_indices, mesh_data.num_uv };

		uint size = sizeof(ranges) + sizeof(float) * mesh_data.num_vertices + sizeof(uint) * mesh_data.num_indices + sizeof(float)* mesh_data.num_uv;// *3;

		char* data = new char[size]; // Allocate
		char* cursor = data;

		// First store ranges
		uint bytes = sizeof(ranges);
		memcpy(cursor, ranges, bytes);

		cursor += bytes;

		//Store vertices
		bytes = sizeof(float) * mesh_data.num_vertices;
		memcpy(cursor, mesh_data.vertices, bytes);

		cursor += bytes;

		//Store indices
		bytes = sizeof(uint) * mesh_data.num_indices;
		memcpy(cursor, mesh_data.indices, bytes);

		cursor += bytes;

		bytes = sizeof(float)* mesh_data.num_uv;
		memcpy(cursor, mesh_data.uv, bytes);

		
		output_file = node->mName.data;


		if (App->fs->SaveFile((char *)data, size, output_file, MESH_FILE))
		{
			//LOG("SCENE IMPORTER: Successfully saved mesh to own format");
		}

		//MEM LEAK

		//RELEASE_ARRAY(data);
		//RELEASE_ARRAY(vertices_);
	}

	for (uint i = 0; i < node->mNumChildren; ++i) {

		LoadModel(scene, node->mChildren[i], output_file);
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

	bytes = sizeof(float) * ret->num_vertices;
	ret->vertices = new float[ret->num_vertices];

	memcpy(ret->vertices, cursor, bytes);

	cursor += bytes;

	bytes = sizeof(uint) * ret->num_indices;
	ret->indices = new uint[ret->num_indices];

	memcpy(ret->indices, cursor, bytes);

	cursor += bytes;

	bytes = sizeof(float) * ret->num_uv;
	ret->uv = new float[ret->num_uv];
	memcpy(ret->uv, cursor, bytes);

	/*// amount of indices / vertices / colors / normals / texture_coords
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
	memcpy(test_vector2.data(), ret->indices, sizeof(uint)*ret->num_indices);*/
	

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

