#include "BaseApp.h"

void dxtk::BaseApp::update()
{
    if(input()->keyDown(VK_ESCAPE))
        window()->close();
}

void dxtk::BaseApp::finalize()
{
    pRootSignature->Release();
    pPipelineState->Release();
}

void dxtk::BaseApp::buildMesh(Mesh& geometry, const std::vector<Vertex>& vertexList)
{
    uint32_t bufferSize = (uint32_t)(vertexList.size() * sizeof(Vertex));

    geometry.unVertByteStride = sizeof(Vertex);
    geometry.unVertBufferSize = bufferSize;


    graphics()->allocate(bufferSize, &geometry.pVertexBufferCPU);
    graphics()->allocate(dxtk::UPLOAD, bufferSize, &geometry.pVertexBufferUpload);
    graphics()->allocate(dxtk::GPU, bufferSize, &geometry.pVertexBufferGPU);

    graphics()->copy(vertexList.data(), bufferSize, geometry.pVertexBufferCPU);
    graphics()->copy(vertexList.data(), bufferSize, geometry.pVertexBufferUpload, geometry.pVertexBufferGPU);
}

void dxtk::BaseApp::buildRootSignature()
{
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters = 0;
    rootSigDesc.pParameters = nullptr;
    rootSigDesc.NumStaticSamplers = 0;
    rootSigDesc.pStaticSamplers = nullptr;
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* pSerializedRootSig = nullptr;
    ID3DBlob* pError = nullptr;

    ThrowIfFailed(D3D12SerializeRootSignature(
        &rootSigDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &pSerializedRootSig,
        &pError));

    ThrowIfFailed(graphics()->device()->CreateRootSignature(
        0,
        pSerializedRootSig->GetBufferPointer(),
        pSerializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&pRootSignature)));
}

void dxtk::BaseApp::buildPipelineState()
{
    D3D12_INPUT_ELEMENT_DESC inputLayout[2] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    ID3DBlob* pVertexShader = nullptr;
    ID3DBlob* pPixelShader = nullptr;

#ifdef NDEBUG
    std::wstring sVertexPath(L"../x64/Release/VertexShader.cso");
    std::wstring sPixelPath(L"../x64/Release/PixelShader.cso");
#else
    std::wstring sVertexPath(L"../x64/Debug/VertexShader.cso");
    std::wstring sPixelPath(L"../x64/Debug/PixelShader.cso");
#endif

    ThrowIfFailed(D3DReadFileToBlob(sVertexPath.c_str(), &pVertexShader));
    ThrowIfFailed(D3DReadFileToBlob(sPixelPath.c_str(), &pPixelShader));

    D3D12_RASTERIZER_DESC rasterizer = {};
    if(bWireframe)
        rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
    else
        rasterizer.FillMode = D3D12_FILL_MODE_SOLID;

    rasterizer.CullMode = D3D12_CULL_MODE_BACK;
    rasterizer.FrontCounterClockwise = FALSE;
    rasterizer.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizer.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizer.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizer.DepthClipEnable = TRUE;
    rasterizer.MultisampleEnable = FALSE;
    rasterizer.AntialiasedLineEnable = FALSE;
    rasterizer.ForcedSampleCount = 0;
    rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    D3D12_BLEND_DESC blender = {};
    blender.AlphaToCoverageEnable = FALSE;
    blender.IndependentBlendEnable = FALSE;
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
    {
        FALSE,FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    for(UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
        blender.RenderTarget[i] = defaultRenderTargetBlendDesc;

    D3D12_DEPTH_STENCIL_DESC depthStencil = {};
    depthStencil.DepthEnable = TRUE;
    depthStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencil.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depthStencil.StencilEnable = FALSE;
    depthStencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    depthStencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
    { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    depthStencil.FrontFace = defaultStencilOp;
    depthStencil.BackFace = defaultStencilOp;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
    pso.pRootSignature = pRootSignature;
    pso.VS = { reinterpret_cast<BYTE*>(pVertexShader->GetBufferPointer()), pVertexShader->GetBufferSize() };
    pso.PS = { reinterpret_cast<BYTE*>(pPixelShader->GetBufferPointer()), pPixelShader->GetBufferSize() };
    pso.BlendState = blender;
    pso.SampleMask = UINT_MAX;
    pso.RasterizerState = rasterizer;
    pso.DepthStencilState = depthStencil;
    pso.InputLayout = { inputLayout, 2 };
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    pso.SampleDesc.Count = graphics()->antialiasing();
    pso.SampleDesc.Quality = graphics()->quality();

    graphics()->device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pPipelineState));

    pVertexShader->Release();
    pPixelShader->Release();
}
