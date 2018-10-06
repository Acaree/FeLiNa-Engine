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
	const aiScene* scene = aiImportFile("Game/FBX/box.FBX",aiProcessPreset_TargetRealtime_MaxQuality);

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
				
				//Need revision when draw a geometry with texture.
				if (new_mesh->HasTextureCoords(data.id_texture))
				{
					data.num_texture = new_mesh->mNumVertices;
					data.texture = new float[data.num_texture*2];

					for (int num_textures = 0; num_textures < new_mesh->mNumVertices; ++num_textures)
					{
						memcpy(&data.texture[num_textures * 2], &new_mesh->mTextureCoords[num_textures],2* sizeof(float));
					}

				}

				//Need revision when draw geometry with color
				if (new_mesh->HasVertexColors(data.id_color))
				{

					data.num_color = new_mesh->GetNumColorChannels();
					data.colors = new float[data.num_color * 4];

					for (int num_colors = 0; num_colors < new_mesh->GetNumColorChannels(); ++num_colors)
					{
						memcpy(&data.colors[num_colors*4], &new_mesh->mColors[num_colors], 4 * sizeof(float));
					}
				}

			}

			glGenBuffers(1, (GLuint*) &(data.id_vertices));
			glBindBuffer(GL_ARRAY_BUFFER, data.id_vertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.num_vertices, data.vertices, GL_STATIC_DRAW);



			glGenBuffers(1, (GLuint*) &(data.id_indices));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.id_indices);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * data.num_indices, data.indices, GL_STATIC_DRAW);

			
			glGenBuffers(1, (GLuint*) &(data.id_texture));
			glBindBuffer(GL_ARRAY_BUFFER, data.id_texture);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.num_texture, data.texture, GL_STATIC_DRAW);



			glGenBuffers(1, (GLuint*) &(data.id_color));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.id_color);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * data.num_color, data.colors, GL_STATIC_DRAW);

			
	}

	

	else
		LOG_GLOBAL("Error loading Scene %s",path);
}