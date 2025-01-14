#include "Application.h"
#include "ModuleFileSystem.h"
#include "Resource.h"
#include "ResourceMesh.h"

ResourceMesh::ResourceMesh(uint UID, ResType type) :Resource(UID, type)
{
}

ResourceMesh::~ResourceMesh()
{
	UnloadFromMemory();
}

void ResourceMesh::setImportDefaults(JSON_Value & importSettings)
{
}

bool ResourceMesh::LoadInMemory()
{
	LoadMesh();

	glGenBuffers(1, (GLuint*)&(id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

bool ResourceMesh::UnloadFromMemory()
{
	RELEASE_ARRAY(vertices);
	RELEASE_ARRAY(normals);
	RELEASE_ARRAY(texCoords);

	glDeleteBuffers(1, &id_indices);
	RELEASE_ARRAY(indices);

	return true;
}

bool ResourceMesh::LoadMesh()
{
	bool ret = true;

	//Get the buffer
	char* buffer = nullptr;
	App->fileSystem->readFile(exported_file.c_str(), &buffer);
	char* cursor = buffer;

	//Load ranges
	uint ranges[5];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;

	num_vertices = ranges[0];
	num_indices = ranges[1];
	num_normals = ranges[2];
	num_texCoords = ranges[3];
	numBones = ranges[4];

	//Load vertices
	if (num_vertices > 0)
	{
		bytes = sizeof(float)*num_vertices * 3;
		vertices = new float[num_vertices * 3];
		memcpy(vertices, cursor, bytes);
		cursor += bytes;
	}

	//Load indices
	if (num_indices > 0)
	{
		bytes = sizeof(uint)*num_indices;
		indices = new uint[num_indices];
		memcpy(indices, cursor, bytes);
		cursor += bytes;
	}

	//Load normals
	if (num_normals > 0)
	{
		bytes = sizeof(float)*num_normals * 3;
		normals = new float[num_normals * 3];
		memcpy(normals, cursor, bytes);
		cursor += bytes;
	}

	//Load tex_coords
	if (num_texCoords > 0)
	{
		bytes = sizeof(float)*num_texCoords * 2;
		texCoords = new float[num_texCoords * 2];
		memcpy(texCoords, cursor, bytes);
		cursor += bytes;
	}

	if (numBones > 0)
	{
		bytes = sizeof(uint)*numBones;
		uint* aux = new uint[numBones];
		memcpy(aux, cursor, bytes);
		cursor += bytes;

		for (int i = 0; i < numBones; i++)
		{
			rBonesUID.push_back(aux[i]);
		}

		RELEASE_ARRAY(aux);
	}

	//Calculate bounding box
	boundingBox.SetNegativeInfinity();
	boundingBox.Enclose((float3*)vertices, num_vertices);

	RELEASE_ARRAY(buffer);
	
	return ret;
}

void ResourceMesh::calculateNormals()
{
	for (int i = 0; i < num_indices; i += 3)
	{
		float x, y, z;
		vec A, B, C, N;

		A = { vertices[indices[i] * 3],vertices[indices[i] * 3 + 1],vertices[indices[i] * 3 + 2] };
		B = { vertices[indices[i + 1] * 3],vertices[indices[i + 1] * 3 + 1],vertices[indices[i + 1] * 3 + 2] };
		C = { vertices[indices[i + 2] * 3],vertices[indices[i + 2] * 3 + 1],vertices[indices[i + 2] * 3 + 2] };
		N = (B - A).Cross(C - A);
		N.Normalize();

		x = (vertices[indices[i] * 3] + vertices[indices[i + 1] * 3] + vertices[indices[i + 2] * 3]) / 3;
		y = (vertices[indices[i] * 3 + 1] + vertices[indices[i + 1] * 3 + 1] + vertices[indices[i + 2] * 3 + 1]) / 3;
		z = (vertices[indices[i] * 3 + 2] + vertices[indices[i + 1] * 3 + 2] + vertices[indices[i + 2] * 3 + 2]) / 3;

		faceNormals.push_back(x);
		faceNormals.push_back(y);
		faceNormals.push_back(z);
		faceNormals.push_back(N.x);
		faceNormals.push_back(N.y);
		faceNormals.push_back(N.z);
	}
}