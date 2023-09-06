#include "Mesh.h"

namespace dxtk
{
	Mesh::Mesh(const char* szName)
		: sName(szName),
		pVertexBufferCPU(nullptr), pVertexBufferUpload(nullptr), pVertexBufferGPU(nullptr),
		unVertByteStride(0),
		unVertBufferSize(0)
	{
		vertexBufferView = { 0 };
	}

	Mesh::~Mesh()
	{
		if(pVertexBufferUpload) pVertexBufferUpload->Release();
		if(pVertexBufferCPU) pVertexBufferCPU->Release();
		if(pVertexBufferGPU) pVertexBufferGPU->Release();
	}

	D3D12_VERTEX_BUFFER_VIEW* Mesh::getVertexBufferView()
	{
		vertexBufferView.BufferLocation = pVertexBufferGPU->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = unVertByteStride;
		vertexBufferView.SizeInBytes = unVertBufferSize;

		return &vertexBufferView;
	}
}
