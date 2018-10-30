#include "ModuleMeshes.h"
#include "Application.h"
#include "Globals.h"
#include "ComponentMesh.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"

#include "Assimp/include/mesh.h"

ModuleMeshes::ModuleMeshes(bool start_enabled): Module(start_enabled)
{
}

Mesh* ModuleMeshes::importMesh(aiMesh mesh)
{
	return nullptr;
}

Mesh* ModuleMeshes::loadMesh(const char* path)
{
	Mesh* ret = new Mesh();
	App->fileSystem->splitPath(path, nullptr, &ret->name, nullptr);

	//Get the buffer
	char* cursor = nullptr;
	App->fileSystem->readFile(path, &cursor);

	//Load ranges
	uint ranges[4];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;

	ret->num_vertices = ranges[0];
	ret->num_indices = ranges[1];
	ret->num_normals = ranges[2];
	ret->num_texCoords = ranges[3];

	//Load vertices
	if (ret->num_vertices > 0)
	{
		bytes = sizeof(float)*ret->num_vertices * 3;
		ret->vertices = new float[ret->num_vertices * 3];
		memcpy(ret->vertices, cursor, bytes);
		cursor += bytes;
	}

	//Load indices
	if (ret->num_indices > 0)
	{
		bytes = sizeof(uint)*ret->num_indices;
		ret->indices = new uint[ret->num_indices];
		memcpy(ret->indices, cursor, bytes);
		cursor += bytes;
	}

	//Load normals
	if (ret->num_normals > 0)
	{
		bytes = sizeof(float)*ret->num_normals * 3;
		ret->normals = new float[ret->num_normals * 3];
		memcpy(ret->normals, cursor, bytes);
		cursor += bytes;
	}

	//Load tex_coords
	if (ret->num_texCoords > 0)
	{
		bytes = sizeof(float)*ret->num_texCoords * 2;
		ret->texCoords = new float[ret->num_texCoords * 2];
		memcpy(ret->texCoords, cursor, bytes);
		cursor += bytes;
	}

	//Load position
	float position[3];
	bytes = sizeof(float) * 3;
	memcpy(position, cursor, bytes);
	ret->position = { position[0],position[1],position[2] };
	cursor += bytes;

	//Load scale
	float scale[3];
	bytes = sizeof(float) * 3;
	memcpy(scale, cursor, bytes);
	ret->scale = { scale[0],scale[1],scale[2] };
	cursor += bytes;

	//Load rotation
	float rotation[4];
	bytes = sizeof(float) * 4;
	memcpy(rotation, cursor, bytes);
	ret->rotation = { rotation[0],rotation[1],rotation[2],rotation[3] };

	//Calculate bounding box
	ret->boundingBox.SetNegativeInfinity();
	ret->boundingBox.Enclose((float3*)ret->vertices, ret->num_vertices);

	return ret;
}

bool ModuleMeshes::saveMesh(Mesh* mesh)
{
	bool ret = true;

	//	Vertices | Indices | Normals(vertices) | Texture coords (verties*2)
	uint ranges[4] = { mesh->num_vertices, mesh->num_indices, mesh->num_normals, mesh->num_texCoords};
	
	//Total size of the buffer
	uint size = sizeof(ranges) + sizeof(float)*mesh->num_vertices*3 + sizeof(uint)*mesh->num_indices + sizeof(float)*mesh->num_normals*3 + sizeof(float)*mesh->num_texCoords * 2;
	size += sizeof(float) * 10;
	char* buffer = new char[size];
	char* cursor = buffer;

	//Store ranges
	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	//Store vertices
	bytes = sizeof(float)*mesh->num_vertices*3;
	memcpy(cursor, mesh->vertices, bytes);
	cursor += bytes;
	
	//Store indices
	bytes = sizeof(uint)*mesh->num_indices;
	memcpy(cursor, mesh->indices, bytes);
	cursor += bytes;

	//Store normals
	bytes = sizeof(float)*mesh->num_normals*3;
	memcpy(cursor, mesh->normals, bytes);
	cursor += bytes;

	//Store tex_coords
	bytes = sizeof(float)*mesh->num_texCoords*2;
	memcpy(cursor, mesh->texCoords, bytes);	
	cursor += bytes;

	//Store position
	float position[3] = { mesh->position.x ,mesh->position.y,mesh->position.z };
	bytes = sizeof(position);
	memcpy(cursor, position, bytes);
	cursor += bytes;

	//Store scale
	float scale[3] = { mesh->scale.x ,mesh->scale.y,mesh->scale.z };
	bytes = sizeof(scale);
	memcpy(cursor, scale, bytes);
	cursor += bytes;

	//Store rotation
	float rotation[4] = { mesh->rotation.x ,mesh->rotation.y,mesh->rotation.z,mesh->rotation.w };
	bytes = sizeof(rotation);
	memcpy(cursor, rotation, bytes);

	App->fileSystem->writeFile((MESHES_FOLDER + mesh->name + MESH_EXTENSION).c_str(), buffer, size, true); //Overwrite must be set to false when scenes save/load is completed

	return ret;
}
