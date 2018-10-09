#include "ModuleFBX.h"

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

ModuleFBX::ModuleFBX(Application*app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleFBX::~ModuleFBX()
{

}

void myCallback(const char *msg, char *userData) {
	LOG_GLOBAL("%s" ,msg);
}

bool ModuleFBX::Start()
{
	aiLogStream stream = aiLogStream();
	stream.callback = myCallback;
	aiAttachLogStream(&stream);
	ilInit();

	return true;
}

bool ModuleFBX::CleanUp()
{
	aiDetachAllLogStreams();

	//TO CHANGE
	/*if (data->indices != nullptr)
		delete[]data->indices;

	if (data->vertices != nullptr)
		delete[]data->vertices;*/

	return true;
}

void ModuleFBX::LoadFbx(const char* path)
{
    LOG_GLOBAL("Inicialization load FBX")

	ModelData* data = new ModelData();

	//TO REVISION
	data->path = path;
	std::string tmp = path;
	data->name = tmp.erase(0,tmp.find_last_of("\\") + 1);


	const aiScene* scene = aiImportFile(path,aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		
			for (int num_meshes = 0; num_meshes < scene->mNumMeshes; ++num_meshes)
			{
				

				aiMesh* new_mesh = scene->mMeshes[num_meshes];
				data->num_vertices = new_mesh->mNumVertices;
				data->vertices = new float[data->num_vertices * 3];
				memcpy(data->vertices, new_mesh->mVertices, sizeof(float)*data->num_vertices * 3);
				LOG_GLOBAL("New mesh with %d verices", data->num_vertices);

				//Geometry

				if (new_mesh->HasFaces())
				{
					data->num_indices = new_mesh->mNumFaces * 3;
					data->indices = new uint[data->num_indices];

					for (uint num_faces = 0; num_faces < new_mesh->mNumFaces; ++num_faces)
					{
						if (new_mesh->mFaces[num_faces].mNumIndices != 3)
						{
							LOG_GLOBAL("Geometry face %i whit %i faces", num_faces, new_mesh->mFaces[num_faces].mNumIndices);
						}
						else
							memcpy(&data->indices[num_faces * 3], new_mesh->mFaces[num_faces].mIndices, 3 * sizeof(uint));

					}
				}
				
				//TO REVISION -> ALL WITH TEXTURE---------------------------------------------
				///Only 1 material for now
				aiMaterial* material = scene->mMaterials[0];

				if (material != nullptr)
				{
					aiString path;
					material->GetTexture(aiTextureType_DIFFUSE,0,&path);

					if (path.length > 0)
					{
						//TO TEST WHEN WE DRAW TEXTURE
						std::string path_location = "FBX/";
						std::string folder = path.data;

						folder.erase(0, folder.find_last_of("\\")+1);
						
						path_location += folder;
					

						ILuint id;
						ilGenImages(1, &id);
						ilBindImage(id);
						ILboolean b = ilLoadImage(path_location.c_str());
						

						data->texture_id = ilutGLBindTexImage();
						
						folder.clear();
						path_location.clear();
						path.Clear();
						//----------------------------------
					}
				}
				///Only 1 material for now
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
				
				//TO REVISION--------------------------------------------------------

				aiMaterial* color_material = scene->mMaterials[new_mesh->mMaterialIndex];

				// if the object don't have color material, we set the color to white 
				//because if not, the object take the last material
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
				//--------------------------------------------------------------------------------


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
			}


			
	}

	

	else
		LOG_GLOBAL("Error loading Scene %s",path);
}

update_status ModuleFBX::PreUpdate(float dt) {
	update_status update_return = UPDATE_CONTINUE;
	module_timer.Start();
	return update_return;
}

update_status ModuleFBX::PostUpdate(float dt) {

	update_status update_return = UPDATE_CONTINUE;

	last_update_ms = module_timer.ReadMs();
	module_timer.Start();

	return update_return;

}

GLuint ModuleFBX::LoadTexture(const char* theFileName)
{
	ILuint imageID;				// Create an image ID as a ULuint

	GLuint textureID;			// Create a texture ID as a GLuint

	ILboolean success;			// Create a flag to keep track of success/failure

	ILenum error;				// Create a flag to keep track of the IL error state

	ilGenImages(1, &imageID); 		// Generate the image ID

	ilBindImage(imageID); 			// Bind the image

	success = ilLoadImage(theFileName); 	// Load the image file

	// If we managed to load the image, then we can start to do things with it...
	if (success)
	{
		// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		// Convert the image into a suitable format to work with
		// NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

		// Quit out if we failed the conversion
		if (!success)
		{
			error = ilGetError();
			std::cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
			exit(-1);
		}

		// Generate a new texture
		glGenTextures(1, &textureID);

		// Bind the texture to a name
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// Set texture interpolation method to use linear interpolation (no MIPMAPS)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Specify the texture specification
		glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
			0,				// Pyramid level (for mip-mapping) - 0 is the top level
			ilGetInteger(IL_IMAGE_FORMAT),	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
			ilGetInteger(IL_IMAGE_WIDTH),	// Image width
			ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
			0,				// Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
			GL_UNSIGNED_BYTE,		// Image data type
			ilGetData());			// The actual image data itself
	}
	else // If we failed to open the image file in the first place...
	{
		error = ilGetError();
		std::cout << "Image load failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
		
	}

	ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

	std::cout << "Texture creation successful." << std::endl;

	return textureID; // Return the GLuint to the texture so you can use it!
}