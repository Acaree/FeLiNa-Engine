#include "ModuleImport.h"

#include "MathGeoLib/MathGeoLib.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "Devil/include/il.h"
#include "Devil/include/ilut.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#pragma comment (lib,"Assimp/libx86/assimp.lib")
#pragma comment (lib, "Devil/libx86/DevIL.lib")
#pragma comment ( lib, "Devil/libx86/ILU.lib" )
#pragma comment ( lib, "Devil/libx86/ILUT.lib" )
#include <string>

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
	ilInit();

	return true;
}

bool ModuleImport::CleanUp()
{
	aiDetachAllLogStreams();

	return true;
}

void ModuleImport::LoadData(const char* path) //TO REVISE THIS FUNCTION BOOL? or not because we have streams to all errors?
{
    LOG("Inicialization load data model")

	
	const aiScene* scene = aiImportFile(path,aiProcessPreset_TargetRealtime_MaxQuality);
	
	if (scene != nullptr && scene->HasMeshes())
	{
		const aiNode* node = scene->mRootNode;

		if (node != nullptr)
		{
			aiQuaternion q;
			aiVector3D scale, pos;
			node->mTransformation.Decompose(scale,q,pos);

			for (int num_meshes = 0; num_meshes < scene->mNumMeshes; ++num_meshes)
			{
				ModelData* data = new ModelData(); //TO REVISE PASS &DATA and not create one?  

				//TO REVISION
				data->path = path;
				std::string tmp = path;
				data->name = tmp.substr(0, tmp.find_last_of("\\") + 1);

				data->position = { pos.x,pos.y,pos.z };
				data->scale = { scale.x,scale.y,scale.z };
				data->rotation = { q.GetEuler().x, q.GetEuler().y, q.GetEuler().z };


				aiMesh* new_mesh = scene->mMeshes[num_meshes];

				data->num_vertices = new_mesh->mNumVertices;
				data->vertices = new float[data->num_vertices * 3];
				memcpy(data->vertices, new_mesh->mVertices, sizeof(float)*data->num_vertices * 3);
				LOG("New mesh with %d vertices", data->num_vertices);

				//Geometry

				if (new_mesh->HasFaces())
				{
					data->num_indices = new_mesh->mNumFaces * 3;
					data->indices = new uint[data->num_indices];

					for (uint num_faces = 0; num_faces < new_mesh->mNumFaces; ++num_faces)
					{
						if (new_mesh->mFaces[num_faces].mNumIndices != 3)
						{
							LOG("Geometry face %i whit %i faces", num_faces, new_mesh->mFaces[num_faces].mNumIndices);
						}
						else
							memcpy(&data->indices[num_faces * 3], new_mesh->mFaces[num_faces].mIndices, 3 * sizeof(uint));

					}
				}

				aiMaterial* material = scene->mMaterials[new_mesh->mMaterialIndex];
				if (new_mesh->HasTextureCoords(0))
				{
					data->num_uv = new_mesh->mNumVertices;
					data->uv = new float[data->num_uv * 2];

					for (uint coords = 0; coords < new_mesh->mNumVertices; ++coords)
					{
						memcpy(&data->uv[coords * 2], &new_mesh->mTextureCoords[0][coords].x, sizeof(float));
						memcpy(&data->uv[(coords * 2) + 1], &new_mesh->mTextureCoords[0][coords].y, sizeof(float));
					}
				}
				//------------------------------------------------------------------------------

				aiMaterial* color_material = scene->mMaterials[new_mesh->mMaterialIndex];

				if (aiGetMaterialColor(color_material, AI_MATKEY_COLOR_AMBIENT, &data->color_4D) == aiReturn_FAILURE || data->color_4D == aiColor4D(0,0,0,1))
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



				glGenBuffers(1, (GLuint*) &(data->id_vertices));
				glBindBuffer(GL_ARRAY_BUFFER, data->id_vertices);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * data->num_vertices, data->vertices, GL_STATIC_DRAW);



				glGenBuffers(1, (GLuint*) &(data->id_indices));
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->id_indices);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * data->num_indices, data->indices, GL_STATIC_DRAW);


				glGenBuffers(1, (GLuint*) &(data->id_uv));
				glBindBuffer(GL_ARRAY_BUFFER, data->id_uv);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * data->num_uv, data->uv, GL_STATIC_DRAW);



				glGenBuffers(1, (GLuint*) &(data->id_color));
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->id_color);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * data->num_color, data->colors, GL_STATIC_DRAW);

				App->renderer3D->AddDataMesh(data); 

				FindTexturePath(material, path, num_meshes);
			}

		}
			
	}
	else
		LOG("Error loading Scene %s",path);
}

//TO REVISION LOGS 
void ModuleImport::FindTexturePath(aiMaterial* material, const char *path, int index)
{
	if (material != nullptr)
	{
		
		aiString texture_name;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_name);

		std::string fbx_path = path;

		//THIS MAGICAL NUMBER HAVE ONE REASEON IF path//Game//test.fbx and want Game// are 5
		std::string fbx_folder = fbx_path.substr(0, fbx_path.find_last_of("\\") + 1) + texture_name.data;
		std::string game_folder = fbx_path.substr(0, fbx_path.find("Game\\") + 5) + texture_name.data;
		std::string felina_folder = fbx_path.substr(0, fbx_path.find("FeLiNa\\") + 7) + texture_name.data;

		bool success = false;
		success = App->texture->LoadTexture(fbx_folder.c_str(),index);

		if (!success)
		{
			success = App->texture->LoadTexture(game_folder.c_str(),index);

			if (!success)
			{
				success = App->texture->LoadTexture(felina_folder.c_str(),index);

			}
		}
	}

}