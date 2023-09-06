#pragma once

#include "Types.h"
#include <d3d12.h>

namespace dxtk
{
	struct Mesh
	{
		String sName;

		ID3DBlob* pVertexBufferCPU;
		ID3D12Resource* pVertexBufferUpload;
		ID3D12Resource* pVertexBufferGPU;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

		uint32_t unVertByteStride;
		uint32_t unVertBufferSize;

		Mesh(const char* szName);
		~Mesh();

		D3D12_VERTEX_BUFFER_VIEW* getVertexBufferView();
	};
}
