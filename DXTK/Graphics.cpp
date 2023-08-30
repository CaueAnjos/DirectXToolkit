#include "Graphics.h"
#include "Error.h"
#include "Window.h"

namespace dxtk
{
	Graphics::Graphics() :
		pFactory(nullptr),
		pDevice(nullptr),
		pCommandQueue(nullptr),
		pCommandList(nullptr),
		pCommandListAlloc(nullptr),
		pFence(nullptr),
		unCurrentFence(0)
	{}

	Graphics::~Graphics()
	{
		waitCommandQueue();

		if(pFence)
			pFence->Release();

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

	void Graphics::Initialize(Window* window)
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

	void Graphics::submitCommands()
	{
		pCommandList->Close();
		ID3D12CommandList* ppCmdsLists[] = { pCommandList };
		pCommandQueue->ExecuteCommandLists(_countof(ppCmdsLists), ppCmdsLists);

		waitCommandQueue();
	}
}