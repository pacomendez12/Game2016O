#include "stdafx.h"
#include "BlenderImporter.h"

#include <iostream>



std::unique_ptr<CMesh> CBlenderImporter::ImportObject(const char * filePath)
{
	Assimp::Importer importer;
#if 0
	aiPropertyStore* props = aiCreatePropertyStore();
	aiSetImportPropertyInteger(props, AI_CONFIG_IMPORT_TER_MAKE_UVS, 1);
	//aiSetImportPropertyFloat(props, AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, g_smoothAngle);
	//aiSetImportPropertyInteger(props, AI_CONFIG_PP_SBP_REMOVE, nopointslines ? aiPrimitiveType_LINE | aiPrimitiveType_POINT : 0);

	aiSetImportPropertyInteger(props, AI_CONFIG_GLOB_MEASURE_TIME, 1);
	unsigned int ppsteps = aiProcess_CalcTangentSpace | // calculate tangents and bitangents if possible
		aiProcess_JoinIdenticalVertices | // join identical vertices/ optimize indexing
		aiProcess_ValidateDataStructure | // perform a full validation of the loader's output
		aiProcess_ImproveCacheLocality | // improve the cache locality of the output vertices
		aiProcess_RemoveRedundantMaterials | // remove redundant materials
		aiProcess_FindDegenerates | // remove degenerated polygons from the import
		aiProcess_FindInvalidData | // detect invalid model data, such as invalid normal vectors
		aiProcess_GenUVCoords | // convert spherical, cylindrical, box and planar mapping to proper UVs
		aiProcess_TransformUVCoords | // preprocess UV transformations (scaling, translation ...)
		aiProcess_FindInstances | // search for instanced meshes and remove them by references to one master
		aiProcess_LimitBoneWeights | // limit bone weights to 4 per vertex
		aiProcess_OptimizeMeshes | // join small meshes, if possible;
		aiProcess_SplitByBoneCount | // split meshes with too many bones. Necessary for our (limited) hardware skinning shader
		0;

	const aiScene *scene = (aiScene*)aiImportFileExWithProperties(filePath,
		ppsteps |
		aiProcess_GenSmoothNormals | // generate smooth normal vectors if not existing
		aiProcess_SplitLargeMeshes | // split large, unrenderable meshes into submeshes
		aiProcess_Triangulate | // triangulate polygons with more than 3 edges
		aiProcess_ConvertToLeftHanded | // convert everything to D3D left handed space
		aiProcess_SortByPType | // make 'clean' meshes which consist of a single typ of primitives
		0,
		NULL,
		props);
#endif
	const aiScene *scene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_Fast);//aiProcessPreset_TargetRealtime_Fast has the configs you'll need
	//const struct aiScene* scene = aiImportFile(filePath, aiProcessPreset_TargetRealtime_Fast);

	if (scene)
	{
		CMesh * mesh = new CMesh;
		unsigned int vertexNumber = scene->mMeshes[0]->mNumVertices;
		mesh->m_Vertices.resize(vertexNumber);
		for (unsigned int i = 0; i < vertexNumber; ++i)
		{
			mesh->m_Vertices[i].Position = {
				scene->mMeshes[0]->mVertices[i].x,
				scene->mMeshes[0]->mVertices[i].y,
				scene->mMeshes[0]->mVertices[i].z,
				1 
			};
		}

		MATRIX4D t;
		//memcpy(&t.v, &scene->mRootNode->mChildren[0]->mTransformation, sizeof(t.v) / sizeof(t.v[0]));
		t.m00 = scene->mRootNode->mChildren[0]->mTransformation.a1;
		t.m01 = scene->mRootNode->mChildren[0]->mTransformation.a2;
		t.m02 = scene->mRootNode->mChildren[0]->mTransformation.a3;
		t.m03 = scene->mRootNode->mChildren[0]->mTransformation.a4;
		t.m10 = scene->mRootNode->mChildren[0]->mTransformation.b1;
		t.m11 = scene->mRootNode->mChildren[0]->mTransformation.b2;
		t.m12 = scene->mRootNode->mChildren[0]->mTransformation.b3;
		t.m13 = scene->mRootNode->mChildren[0]->mTransformation.b4;
		t.m20 = scene->mRootNode->mChildren[0]->mTransformation.c1;
		t.m21 = scene->mRootNode->mChildren[0]->mTransformation.c2;
		t.m22 = scene->mRootNode->mChildren[0]->mTransformation.c3;
		t.m23 = scene->mRootNode->mChildren[0]->mTransformation.c4;
		t.m30 = scene->mRootNode->mChildren[0]->mTransformation.d1;
		t.m31 = scene->mRootNode->mChildren[0]->mTransformation.d2;
		t.m32 = scene->mRootNode->mChildren[0]->mTransformation.d3;
		t.m33 = scene->mRootNode->mChildren[0]->mTransformation.d4;

		mesh->m_World = Transpose(t);

		mesh->m_Indices.resize(scene->mMeshes[0]->mNumFaces * scene->mMeshes[0]->mFaces[0].mNumIndices);
		for (unsigned long i = 0; i < scene->mMeshes[0]->mNumFaces; ++i)
		{
			for (unsigned long j = 0; j < scene->mMeshes[0]->mFaces[i].mNumIndices; ++j)
			{
				mesh->m_Indices[i*scene->mMeshes[0]->mFaces[i].mNumIndices + j] = scene->mMeshes[0]->mFaces[i].mIndices[j];
			}
		}

		for (unsigned int i = 0; i < vertexNumber; ++i)
		{
			VECTOR4D TexCoord = { 0,0,0,0 };
			TexCoord.x = mesh->m_Vertices[i].Position.x;
			TexCoord.y = mesh->m_Vertices[i].Position.z;
			TexCoord.z = mesh->m_Vertices[i].Position.y;
			TexCoord = Normalize(TexCoord);
			TexCoord.x = TexCoord.x * 0.5 + 0.5;
			TexCoord.y = TexCoord.y * 0.5 + 0.5;

			mesh->m_Vertices[i].TexCoord = TexCoord;
		}

		//m_Meshes[i].Optimize();
		mesh->BuildTangentSpaceFromTexCoordsIndexed(true);
		mesh->GenerarCentroides();

		strcpy_s(mesh->m_cName, scene->mMeshes[0]->mName.C_Str());

		return std::unique_ptr<CMesh>(mesh);
	}
	std::cout << "No se pudo cargar la escena del archivo de blender " << filePath << std::endl;
	return std::unique_ptr<CMesh>();
}
