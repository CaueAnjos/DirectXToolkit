#pragma once
#include "DXTK.h"

#include <D3DCompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

namespace dxtk
{
	class BaseApp : public App
	{
	public:
		virtual void update() override;
		virtual	void finalize() override;
		
		void buildMesh(Mesh& geometry, const std::vector<Vertex>& vertexList);
		void buildRootSignature();
		void buildPipelineState();

		std::unique_ptr<dxtk::Mesh> pGeometry;

		bool bWireframe;
	protected:
		ID3D12RootSignature* pRootSignature;
		ID3D12PipelineState* pPipelineState;
	};
}
