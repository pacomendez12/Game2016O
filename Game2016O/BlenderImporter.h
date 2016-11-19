#pragma once

#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <memory>

#include "Graphics\Mesh.h"

class CBlenderImporter
{
public:
	CBlenderImporter() = delete;
	~CBlenderImporter() = delete;
	static std::unique_ptr<CMesh> ImportObject(const char *filePath);
};

