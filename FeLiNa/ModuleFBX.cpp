#include "ModuleFBX.h"


ModuleFBX::ModuleFBX(Application*app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleFBX::~ModuleFBX()
{

}

bool ModuleFBX::Start()
{
	stream = aiLogStream();
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
		aiNode* root = scene->mRootNode;

		if (root != nullptr)
		{
			for (uint num_children = 0; num_children < root->mNumChildren; ++num_children)
			{
				if (root->mChildren[num_children]->mNumMeshes <= 0)
				{
					LOG_GLOBAL("Can't load children %i, number of meshes are %i ", num_children, root->mChildren[num_children]->mNumMeshes);
					//break;
				}
				else
				{
					//For Meshes verices --- ¿root->mChildren[i] == node?
					for (int num_meshes = 0; num_meshes < root->mChildren[num_children]->mNumMeshes; ++num_meshes)
					{
						aiMesh* new_mesh = scene->mMeshes[root->mChildren[num_children]->mMeshes[num_children]];
						data.num_vertices = new_mesh->mNumVertices;
						data.vertices = new float[data.num_vertices * 3];
						memcpy(data.vertices,new_mesh->mVertices,sizeof(float)*data.num_vertices*3);
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
						else
						{
							LOG_GLOBAL("Mesh %i have %i faces can't loaded",num_children,new_mesh->mNumFaces);
							//break;
						}


					}

					


				}
			}
		}
		else
			LOG_GLOBAL("Root are nullptr");
	}
	else
		LOG_GLOBAL("Error loading Scene %s",path);


}