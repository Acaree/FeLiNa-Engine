#include "ModuleImport.h"
#include "Glew/include/glew.h" 
#include "SDL/include/SDL_opengl.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleTexture.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "ModuleWindow.h"
#include <string>
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentTexture.h"
#pragma comment (lib,"Assimp/libx86/assimp.lib")



ModuleImport::ModuleImport(Application*app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleImport::~ModuleImport()
{

}

void myCallback(const char *msg, char *userData)
{
	LOG("%s" ,msg);
}

bool ModuleImport::Start()
{
	aiLogStream stream = aiLogStream();
	stream.callback = myCallback;
	aiAttachLogStream(&stream);


	return true;
}

bool ModuleImport::CleanUp()
{
	aiDetachAllLogStreams();

	return true;
}

bool ModuleImport::LoadData(const char* path) 
{
	LOG("Inicialization load data model");


	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	
	

	std::string tmp = path;
	tmp = tmp.erase(0, tmp.find_last_of("\\") + 1);
	tmp = tmp.substr(0, tmp.find_last_of("."));
	//TO REVISE-> ¿Create a function that convert a const char* to char*?
	int length = strlen(tmp.c_str());
	char* temp = new char[length + 1];
	strcpy(temp, tmp.c_str());
	temp[length] = '\0';

	GameObject* obj2 = new GameObject(nullptr);
	obj2->SetName(temp);
	if (scene != nullptr)
	{
		aiNode* rootNode = scene->mRootNode;
		ComponentTransform* trans = new ComponentTransform(nullptr);
		LoadModel(scene, rootNode,path, obj2, trans);
		obj2->SetComponent(trans);
	}

	obj2->SetParent(App->scene->root_object);
	
	App->scene->root_object->AddChildren(obj2);


	//To change-> false and show.
	return true;

}

void ModuleImport::LoadModel(const aiScene* scene, aiNode* node, const char* path, GameObject* obj, ComponentTransform* trans)
{

	//Creating a game object to set data
	GameObject* game_object = new GameObject(nullptr);
	game_object->SetName(node->mName.data);
	aiQuaternion q;
	aiVector3D scale, pos;
	node->mTransformation.Decompose(scale, q, pos);

	ComponentTransform* component_trans = new ComponentTransform(nullptr, math::float3(pos.x, pos.y, pos.z), math::float3(q.GetEuler().x, q.GetEuler().y, q.GetEuler().z), math::float3(scale.x, scale.y, scale.z));
	component_trans->SumPosition(trans->GetPosition());
	component_trans->SumRotation(trans->GetRotation());
	component_trans->SumScale(trans->GetScale());


	if (node->mNumMeshes > 0)
	{
		//Create a game object components
		ComponentMesh* component_mesh = nullptr;
		ComponentTexture* component_texture = nullptr;
		ComponentTransform* tr = component_trans;

		tr->SetParent(game_object);
		
	

		Mesh* mesh_data = new Mesh();

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

			component_texture = FindTexturePath(material, path, 0);



			//Normals
			if (new_mesh->HasNormals())
			{
				mesh_data->num_normals = new_mesh->mNumVertices;
				mesh_data->normals = new float[mesh_data->num_normals * 3];
				memcpy(mesh_data->normals, new_mesh->mNormals, sizeof(float) * mesh_data->num_normals * 3);
			}

			//Add the mesh component
			GenerateBufferData(mesh_data);

			App->renderer3D->AddDataMesh(mesh_data);

			component_mesh = App->renderer3D->CreateComponentMesh();

		}

		//CREATE AABB
		game_object->AddBoundingBox(mesh_data);
		game_object->SetComponent(tr);
		game_object->SetComponent(component_mesh);
		game_object->SetComponent(component_texture);

		game_object->SetParent(obj);
		obj->AddChildren(game_object);

	}
	else
	{
		game_object = obj;
	}




	for (uint i = 0; i < node->mNumChildren; ++i)
	{
		LoadModel(scene, node->mChildren[i], path, game_object,component_trans);
	}
		

	
	
	
}

void ModuleImport::GenerateBufferData(Mesh* mesh_data)
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

	glGenBuffers(1, (GLuint*)&(mesh_data->id_normals));
	glBindBuffer(GL_ARRAY_BUFFER, mesh_data->id_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh_data->num_normals, mesh_data->normals, GL_STATIC_DRAW);

	/*glGenBuffers(1, (GLuint*) &(data->id_color));
	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->id_color);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * data->num_color, data->colors, GL_STATIC_DRAW);
	*/

}

//TO REVISION LOGS 
ComponentTexture* ModuleImport::FindTexturePath(aiMaterial* material, const char *path, int index)
{
	ComponentTexture* texture = nullptr;
	bool succes = false;

	if (material != nullptr)
	{
		aiString texture_name;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_name);

		std::string fbx_path = path;

		std::string texture_folder = fbx_path.substr(0, fbx_path.find_last_of("\\") + 1)  + texture_name.data;
		std::string fbx_folder = fbx_path.substr(0, fbx_path.find_last_of("\\") + 1) + texture_name.data;
		std::string game_folder = fbx_path.substr(0, fbx_path.find("Game\\") + 5) + texture_name.data;
		std::string felina_folder = fbx_path.substr(0, fbx_path.find("FeLiNa\\") + 7) + texture_name.data;

		
		succes = App->texture->LoadTexture(texture_folder.c_str(), index);

		if (!succes)
		{
			succes = App->texture->LoadTexture(fbx_folder.c_str(), index);

			if (!succes)
			{
				succes = App->texture->LoadTexture(game_folder.c_str(), index);

				if (!succes)
				{
					succes = App->texture->LoadTexture(felina_folder.c_str(), index);

				}
			}
		}
		
		if (succes)
			texture = App->texture->CreateComponentTexture();
	}

	return texture;
}

