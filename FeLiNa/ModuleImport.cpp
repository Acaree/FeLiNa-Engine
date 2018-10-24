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

bool ModuleImport::LoadData(const char* path) //TO REVISE THIS FUNCTION BOOL? or not because we have streams to all errors?
{
	LOG("Inicialization load data model");


	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	

	if (scene != nullptr && scene->HasMeshes())
	{
		const aiNode* node = scene->mRootNode;

		for (int num_children = 0; num_children < node->mNumChildren; ++num_children)
		{
			App->scene->root_object->AddChildren(LoadModel(scene, node->mChildren[num_children], path));
		}

	}

	//To change-> false and show.
	return true;

}

GameObject* ModuleImport::LoadModel(const aiScene* scene, aiNode* node, const char* path)
{

	//Creating a game object to set data
	GameObject* game_object = new GameObject(nullptr);

	//Game Object name
	game_object->SetName((char*)node->mName.C_Str());

	if (node->mNumMeshes > 0)
	{
		//Create a game object components
		ComponentMesh* component_mesh = new ComponentMesh(game_object);
		Mesh* mesh_data = new Mesh();
		ComponentTransform* component_transform = new ComponentTransform(game_object);

		aiQuaternion q;
		aiVector3D scale, pos;

		node->mTransformation.Decompose(scale, q, pos);

		component_transform->SetPosition(float3(pos.x, pos.y, pos.z));
		component_transform->SetQuaternion(Quat(q.x, q.y, q.z, q.w));
		component_transform->SetScale(float3(scale.x, scale.y, scale.z));

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

			//Normals
			if (new_mesh->HasNormals())
			{
				mesh_data->num_normals = new_mesh->mNumVertices;
				mesh_data->normals = new float[mesh_data->num_normals * 3];
				memcpy(mesh_data->normals, new_mesh->mNormals, sizeof(float) * mesh_data->num_normals * 3);


			}


			//Don't need color
			/*aiMaterial* color_material = scene->mMaterials[new_mesh->mMaterialIndex];

			if (aiGetMaterialColor(color_material, AI_MATKEY_COLOR_AMBIENT, &data->color_4D) == aiReturn_FAILURE || data->color_4D == aiColor4D(0, 0, 0, 1))
			{
				data->color_4D = { 255.0f,255.0f,255.0f,255.0f };
			}

			aiColor4D* colors_mesh = *new_mesh->mColors;

			if (colors_mesh != nullptr)
			{
				data->colors = new float[data->num_vertices * 3];
				for (int num_color = 0; num_color < data->num_vertices; ++num_color)
				{
					memcpy(data->colors, &colors_mesh[num_color], sizeof(float)*data->num_vertices * 3);
				}
			}

						*/

			//Add the mesh component

			GenerateBufferData(mesh_data);

			component_mesh->SetMesh(mesh_data);

		}

		game_object->SetComponent(component_transform);
		game_object->SetComponent(component_mesh);
		//Ad to game object, mesh component.
		

		/*if (node->mNumMeshes > 0)

			aiQuaternion q;
		aiVector3D scale, pos;

		node->mTransformation.Decompose(scale, q, pos);

		ModelData* data = new ModelData();


		data->path = path;
		std::string tmp = path;
		data->name = tmp.erase(0, tmp.find_last_of("\\") + 1);
		tmp.clear();
		data->position = { pos.x,pos.y,pos.z };
		data->scale = { scale.x,scale.y,scale.z };
		data->rotation = Quat(q.x, q.y, q.z, q.w);

		App->renderer3D->AddDataMesh(data);
		//TO revision best wave?
		FindTexturePath(material, path, num_meshes);*/

	}

	/*if (node->mChildren[num_children]->mNumChildren > 0)
	{

	}*/

	//Recursive search for all meshes in all children meshes
	for (int meshes_children = 0; meshes_children < node->mNumChildren; ++meshes_children)
		game_object->AddChildren(LoadModel(scene, node->mChildren[meshes_children], path));

	return game_object;
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
void ModuleImport::FindTexturePath(aiMaterial* material, const char *path, int index)
{
	if (material != nullptr)
	{
		
		aiString texture_name;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_name);

		std::string fbx_path = path;

		std::string texture_folder = fbx_path.substr(0, fbx_path.find_last_of("\\") + 1)  + texture_name.data;
		std::string fbx_folder = fbx_path.substr(0, fbx_path.find_last_of("\\") + 1) + texture_name.data;
		std::string game_folder = fbx_path.substr(0, fbx_path.find("Game\\") + 5) + texture_name.data;
		std::string felina_folder = fbx_path.substr(0, fbx_path.find("FeLiNa\\") + 7) + texture_name.data;

		bool success = false;
		success = App->texture->LoadTexture(texture_folder.c_str(), index);

		if (!success)
		{
			success = App->texture->LoadTexture(fbx_folder.c_str(), index);

			if (!success)
			{
				success = App->texture->LoadTexture(game_folder.c_str(), index);

				if (!success)
				{
					success = App->texture->LoadTexture(felina_folder.c_str(), index);

				}
			}
		}
	}

}