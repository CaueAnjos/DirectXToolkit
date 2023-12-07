#include "Graphics.h"
#include "Error.h"
#include "Window.h"

namespace dxtk
{
	Graphics::Graphics()
	{
		unBackBufferCount = 2;  
		backBufferIndex = 0;    
		unAntialiasing = 1;     
		unQuality = 0;          
		bVSync = false;

		bgColor[0] = 0.f;
		bgColor[1] = 0.f;
		bgColor[2] = 0.f;
		bgColor[3] = 0.f;

		pFactory = nullptr;
		pDevice = nullptr;
		pSwapChain = nullptr;
		pCommandQueue = nullptr;
		pCommandList = nullptr;
		pCommandListAlloc = nullptr;

		ppRenderTargets = new ID3D12Resource* [unBackBufferCount] { nullptr };
		pDepthStencil = nullptr;
		pRenderTargetHeap = nullptr;
		pDepthStencilHeap = nullptr;
		rtDescriptorSize = 0;
		ZeroMemory(&viewport, sizeof(viewport));
		ZeroMemory(&scissorRect, sizeof(scissorRect));

		pFence = nullptr;
		unCurrentFence = 0;
	}

	Graphics::~Graphics()
	{
		waitCommandQueue();

		if(pDepthStencil)
			pDepthStencil->Release();

		if(ppRenderTargets)
		{
			for(size_t i = 0; i < unBackBufferCount; ++i)
			{
				if(ppRenderTargets[i])
					ppRenderTargets[i]->Release();
			}
			delete[] ppRenderTargets;
		}

		if(pFence)
			pFence->Release();

		if(pDepthStencilHeap)
			pDepthStencilHeap->Release();

		if(pRenderTargetHeap)
			pRenderTargetHeap->Release();

		if(pSwapChain)
		{

			pSwapChain->SetFullscreenState(false, NULL);
			pSwapChain->Release();
		}

		if(pCommandList)
			pCommandList->Release();

		if(pCommandListAlloc)
			pCommandListAlloc->Release();

		if(pCommandQueue)
			pCommandQueue->Release();

		if(pDevice)
			pDevice->Release();

		if(pFactory)
			pFactory->Release();
	}

	void Graphics::logHardwareInfo()
	{
		const uint32_t unBytesinMegaByte = 1048576U;

		IDXGIAdapter* pAdapter = nullptr;
		if(pFactory->EnumAdapters(0, &pAdapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC desc;
			pAdapter->GetDesc(&desc);

			std::wstringstream text;
			text << L"---> Placa de vídeo: " << desc.Description << L"\n";
			OutputDebugStringW(text.str().c_str());
		}

		IDXGIAdapter4* pAdapter4 = nullptr;
		if(SUCCEEDED(pAdapter->QueryInterface(IID_PPV_ARGS(&pAdapter4))))
		{
			DXGI_QUERY_VIDEO_MEMORY_INFO memInfo;
			pAdapter4->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memInfo);

			std::wstringstream text;
			text << L"---> Memória de vídeo (livre): " << memInfo.Budget / unBytesinMegaByte << L"MB\n";
			text << L"---> Memória de vídeo (usada): " << memInfo.CurrentUsage / unBytesinMegaByte << L"MB\n";
			OutputDebugStringW(text.str().c_str());

			pAdapter4->Release();
		}

		D3D_FEATURE_LEVEL featureLevels[9] =
		{
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelsInfo;
		featureLevelsInfo.NumFeatureLevels = 9;
		featureLevelsInfo.pFeatureLevelsRequested = featureLevels;

		pDevice->CheckFeatureSupport(
			D3D12_FEATURE_FEATURE_LEVELS,
			&featureLevelsInfo,
			sizeof(featureLevelsInfo));

		{
			std::wstringstream text;
			text << L"---> Feature Level: ";
			switch(featureLevelsInfo.MaxSupportedFeatureLevel)
			{
			case D3D_FEATURE_LEVEL_12_1: text << L"12_1\n"; break;
			case D3D_FEATURE_LEVEL_12_0: text << L"12_0\n"; break;
			case D3D_FEATURE_LEVEL_11_1: text << L"11_1\n"; break;
			case D3D_FEATURE_LEVEL_11_0: text << L"11_0\n"; break;
			case D3D_FEATURE_LEVEL_10_1: text << L"10_1\n"; break;
			case D3D_FEATURE_LEVEL_10_0: text << L"10_0\n"; break;
			case D3D_FEATURE_LEVEL_9_3:  text << L"9_3\n";  break;
			case D3D_FEATURE_LEVEL_9_2:  text << L"9_2\n";  break;
			case D3D_FEATURE_LEVEL_9_1:  text << L"9_1\n";  break;
			}
			OutputDebugStringW(text.str().c_str());
		}

		IDXGIOutput* pOutput = nullptr;
		if(pAdapter->EnumOutputs(0, &pOutput) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_OUTPUT_DESC desc;
			pOutput->GetDesc(&desc);

			std::wstringstream text;
			text << L"---> Monitor: " << desc.DeviceName << L"\n";
			OutputDebugStringW(text.str().c_str());
		}

		uint32_t dpi = GetDpiForSystem();
		uint32_t screenWidth = GetSystemMetricsForDpi(SM_CXSCREEN, dpi);
		uint32_t screenHeight = GetSystemMetricsForDpi(SM_CYSCREEN, dpi);

		DEVMODE devMode = { 0 };
		devMode.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
		uint32_t refresh = devMode.dmDisplayFrequency;

		std::wstringstream text;
		text << L"---> Resolução: " << screenWidth << L"x" << screenHeight << L" " << refresh << L" Hz\n";
		OutputDebugStringW(text.str().c_str());

		if(pAdapter) pAdapter->Release();
		if(pOutput) pOutput->Release();
	}

	void Graphics::initialize(Window* window)
	{
		uint32_t factoryFlags = 0;

#ifdef _DEBUG
		factoryFlags = DXGI_CREATE_FACTORY_DEBUG;

		ID3D12Debug* debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
#endif
		ThrowIfFailed(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&pFactory)));

		if FAILED(D3D12CreateDevice(
			nullptr,                                
			D3D_FEATURE_LEVEL_11_0,                 
			IID_PPV_ARGS(&pDevice)))                 
		{ 
			IDXGIAdapter* warp;
			ThrowIfFailed(pFactory->EnumWarpAdapter(IID_PPV_ARGS(&warp)));

			ThrowIfFailed(D3D12CreateDevice(
				warp,                               
				D3D_FEATURE_LEVEL_11_0,             
				IID_PPV_ARGS(&pDevice)));            

			warp->Release();

			/*
			informa uso de um disposito WARP:
			implementa as funcionalidades do 
			D3D12 em software (lento)
			*/
			OutputDebugString("---> Usando Adaptador WARP: não há suporte ao D3D12\n");
		}

		// exibe informações do hardware gráfico no Output do Visual Studio
#ifdef _DEBUG
		logHardwareInfo();
#endif 
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		ThrowIfFailed(pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pCommandQueue)));

		ThrowIfFailed(pDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&pCommandListAlloc)));

		ThrowIfFailed(pDevice->CreateCommandList(
			0,										
			D3D12_COMMAND_LIST_TYPE_DIRECT,			
			pCommandListAlloc,						
			nullptr,								
			IID_PPV_ARGS(&pCommandList)));			

		ThrowIfFailed(pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence)));

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = window->width();
		swapChainDesc.Height = window->height();
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.SampleDesc.Count = unAntialiasing;
		swapChainDesc.SampleDesc.Quality = unQuality;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = unBackBufferCount;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;


		ThrowIfFailed(pFactory->CreateSwapChainForHwnd(
			pCommandQueue,                            
			window->id(),                           
			&swapChainDesc,                          
			nullptr,                                
			nullptr,                                
			&pSwapChain));                           

		D3D12_DESCRIPTOR_HEAP_DESC renderTargetHeapDesc = {};
		renderTargetHeapDesc.NumDescriptors = unBackBufferCount;
		renderTargetHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		renderTargetHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(pDevice->CreateDescriptorHeap(&renderTargetHeapDesc, IID_PPV_ARGS(&pRenderTargetHeap)));

		D3D12_CPU_DESCRIPTOR_HANDLE rtHandle = pRenderTargetHeap->GetCPUDescriptorHandleForHeapStart();

		rtDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		for(uint32_t i = 0; i < unBackBufferCount; ++i)
		{
			pSwapChain->GetBuffer(i, IID_PPV_ARGS(&ppRenderTargets[i]));
			pDevice->CreateRenderTargetView(ppRenderTargets[i], nullptr, rtHandle);
			rtHandle.ptr += rtDescriptorSize;
		}

		D3D12_RESOURCE_DESC depthStencilDesc = {};
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = window->width();
		depthStencilDesc.Height = window->height();
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = unAntialiasing;
		depthStencilDesc.SampleDesc.Quality = unQuality;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		// propriedades da Heap do buffer Depth/Stencil
		D3D12_HEAP_PROPERTIES dsHeapProperties = {};
		dsHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		dsHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		dsHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		dsHeapProperties.CreationNodeMask = 1;
		dsHeapProperties.VisibleNodeMask = 1;

		// descreve valores para limpeza do Depth/Stencil buffer
		D3D12_CLEAR_VALUE optmizedClear = {};
		optmizedClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		optmizedClear.DepthStencil.Depth = 1.0f;
		optmizedClear.DepthStencil.Stencil = 0;

		// cria um buffer Depth/Stencil
		ThrowIfFailed(pDevice->CreateCommittedResource(
			&dsHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optmizedClear,
			IID_PPV_ARGS(&pDepthStencil)));

		// descreve e cria uma heap para o descritor tipo Depth/Stencil (DS)
		D3D12_DESCRIPTOR_HEAP_DESC depthstencilHeapDesc = {};
		depthstencilHeapDesc.NumDescriptors = 1;
		depthstencilHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		depthstencilHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(pDevice->CreateDescriptorHeap(&depthstencilHeapDesc, IID_PPV_ARGS(&pDepthStencilHeap)));

		D3D12_CPU_DESCRIPTOR_HANDLE dsHandle = pDepthStencilHeap->GetCPUDescriptorHandleForHeapStart();

		pDevice->CreateDepthStencilView(pDepthStencil, nullptr, dsHandle);

		// faz a transição do estado inicial do recurso para ser usado como buffer de profundidade
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = pDepthStencil;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		pCommandList->ResourceBarrier(1, &barrier);

		submitCommands();

		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = float(window->width());
		viewport.Height = float(window->height());
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		scissorRect = { 0, 0, (long)window->width(), (long)window->height() };

		// TODO: use the convert method
		Color color = window->color();

		bgColor[0] = color.r / 255.0f;
		bgColor[1] = color.g / 255.0f;
		bgColor[2] = color.b / 255.0f;
		bgColor[3] = 1.0f;                     
	}

	void Graphics::clear(ID3D12PipelineState* pso)
	{
		pCommandListAlloc->Reset();
		pCommandList->Reset(pCommandListAlloc, pso);

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = ppRenderTargets[backBufferIndex];
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		pCommandList->ResourceBarrier(1, &barrier);

		pCommandList->RSSetViewports(1, &viewport);
		pCommandList->RSSetScissorRects(1, &scissorRect);
   
		D3D12_CPU_DESCRIPTOR_HANDLE dsHandle = pDepthStencilHeap->GetCPUDescriptorHandleForHeapStart();
		D3D12_CPU_DESCRIPTOR_HANDLE rtHandle = pRenderTargetHeap->GetCPUDescriptorHandleForHeapStart();
		rtHandle.ptr += SIZE_T(backBufferIndex) * SIZE_T(rtDescriptorSize);
		pCommandList->ClearRenderTargetView(rtHandle, bgColor, 0, nullptr);
		pCommandList->ClearDepthStencilView(dsHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		pCommandList->OMSetRenderTargets(1, &rtHandle, true, &dsHandle);
	}

	void Graphics::present()
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = ppRenderTargets[backBufferIndex];
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		pCommandList->ResourceBarrier(1, &barrier);

		submitCommands();

		pSwapChain->Present(bVSync, 0);
		backBufferIndex = (backBufferIndex + 1) % unBackBufferCount;
	}

	bool Graphics::waitCommandQueue()
	{
		unCurrentFence++;

		if(FAILED(pCommandQueue->Signal(pFence, unCurrentFence)))
			return false;

		if(pFence->GetCompletedValue() < unCurrentFence)
		{
			HANDLE eventHandle = CreateEventEx(NULL, NULL, NULL, EVENT_ALL_ACCESS);

			if(eventHandle)
			{ 
				if(FAILED(pFence->SetEventOnCompletion(unCurrentFence, eventHandle)))
					return false;

				WaitForSingleObject(eventHandle, INFINITE);
				CloseHandle(eventHandle);
			}
		}

		return true;
	}

	void Graphics::resetCommands()
	{
		pCommandList->Reset(pCommandListAlloc, nullptr);
	}

	void Graphics::submitCommands()
	{
		pCommandList->Close();
		ID3D12CommandList* ppCmdsLists[] = { pCommandList };
		pCommandQueue->ExecuteCommandLists(_countof(ppCmdsLists), ppCmdsLists);

		waitCommandQueue();
	}

	void Graphics::allocate(uint32_t sizeInBytes, ID3DBlob** resource)
	{
		D3DCreateBlob(sizeInBytes, resource);
	}

	void Graphics::allocate(uint32_t type, uint32_t sizeInBytes, ID3D12Resource** resource)
	{
		D3D12_HEAP_PROPERTIES bufferProp = {};
		bufferProp.Type = D3D12_HEAP_TYPE_DEFAULT;
		bufferProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		bufferProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		bufferProp.CreationNodeMask = 1;
		bufferProp.VisibleNodeMask = 1;

		if(type == ALLOCATION_TYPE::UPLOAD)
			bufferProp.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC bufferDesc = {};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Alignment = 0;
		bufferDesc.Width = sizeInBytes;
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_COMMON;

		if(type == ALLOCATION_TYPE::UPLOAD)
			initState = D3D12_RESOURCE_STATE_GENERIC_READ;

		ThrowIfFailed(pDevice->CreateCommittedResource(
			&bufferProp,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			initState,
			nullptr,
			IID_PPV_ARGS(resource)));
	}

	void Graphics::copy(const void* vertices, uint32_t sizeInBytes, ID3DBlob* bufferCPU)
	{
		CopyMemory(bufferCPU->GetBufferPointer(), vertices, sizeInBytes);
	}

	void Graphics::copy(const void* vertices, uint32_t sizeInBytes, ID3D12Resource* bufferUpload, ID3D12Resource* bufferGPU)
	{
		D3D12_SUBRESOURCE_DATA vertexSubResourceData = {};
		vertexSubResourceData.pData = vertices;
		vertexSubResourceData.RowPitch = sizeInBytes;
		vertexSubResourceData.SlicePitch = sizeInBytes;

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT layouts;
		uint32_t numRows;
		uint64_t rowSizesInBytes;
		uint64_t requiredSize = 0;

		D3D12_RESOURCE_DESC bufferGPUDesc = bufferGPU->GetDesc();

		pDevice->GetCopyableFootprints(
			&bufferGPUDesc,
			0, 1, 0, &layouts, &numRows,
			&rowSizesInBytes, &requiredSize);

		BYTE* pData;
		bufferUpload->Map(0, nullptr, (void**)&pData);

		D3D12_MEMCPY_DEST DestData =
		{
			pData + layouts.Offset,
			layouts.Footprint.RowPitch,
			layouts.Footprint.RowPitch * uint64_t(numRows)
		};

		for(uint32_t z = 0; z < layouts.Footprint.Depth; ++z)
		{
			BYTE* destSlice = (BYTE*)(DestData.pData) + DestData.SlicePitch * z;

			const BYTE* srcSlice = (const BYTE*)(vertexSubResourceData.pData) + vertexSubResourceData.SlicePitch * z;

			for(uint32_t y = 0; y < numRows; ++y)
				memcpy(destSlice + DestData.RowPitch * y,
					srcSlice + vertexSubResourceData.RowPitch * y,
					(size_t)rowSizesInBytes);
		}

		bufferUpload->Unmap(0, nullptr);

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = bufferGPU;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		pCommandList->ResourceBarrier(1, &barrier);

		pCommandList->CopyBufferRegion(
			bufferGPU,
			0,
			bufferUpload,
			layouts.Offset,
			layouts.Footprint.Width);

		barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = bufferGPU;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		pCommandList->ResourceBarrier(1, &barrier);
	}
}