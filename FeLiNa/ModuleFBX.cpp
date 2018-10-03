#include "ModuleFBX.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

ModuleFBX::ModuleFBX(Application*app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleFBX::~ModuleFBX()
{

}

bool ModuleFBX::Start()
{
	aiLogStream stream = aiLogStream();
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool ModuleFBX::CleanUp()
{
	aiDetachAllLogStreams();

	if (data.indices != nullptr)
		delete[]data.indices;

	if (data.vertices != nullptr)
		delete[]data.vertices;

	return true;
}

void ModuleFBX::LoadFbx(const char* path)
{
    LOG_GLOBAL("Inicialization load FBX")
	const aiScene* scene = aiImportFile(path,aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		
			for (int num_meshes = 0; num_meshes < scene->mNumMeshes; ++num_meshes)
			{
				aiMesh* new_mesh = scene->mMeshes[num_meshes];
				data.num_vertices = new_mesh->mNumVertices;
				data.vertices = new float[data.num_vertices * 3];
				memcpy(data.vertices, new_mesh->mVertices, sizeof(float)*data.num_vertices * 3);
				LOG_GLOBAL("New mesh with %d verices", data.num_vertices);

				//Geometry

				if (new_mesh->HasNormals())
				{
					data.num_indices = new_mesh->mNumFaces * 3;
					data.indices = new uint[data.num_indices];

					for (uint num_faces = 0; num_faces < new_mesh->mNumFaces; num_faces++)
					{
						if (new_mesh->mFaces[num_faces].mNumIndices != 3)
						{
							LOG_GLOBAL("Geometry face %i whit %i faces", num_faces, new_mesh->mFaces[num_faces].mNumIndices);
						}
						else
							memcpy(&data.indices[num_faces * 3], new_mesh->mFaces[num_faces].mIndices, 3 * sizeof(uint));

					}
				}

			}
	}
	else
		LOG_GLOBAL("Error loading Scene %s",path);
}