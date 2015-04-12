#include "Components\MeshRenderer.h"
#include "ResourceHolder.h"

void FMeshRenderer::LinkToMesh(const char* MeshName)
{
	Mesh = &SMeshHolder::Get(MeshName);
	Mesh->Renderers.push_back(this);
}