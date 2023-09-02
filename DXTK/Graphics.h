#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include "Types.h"

namespace dxtk
{
    class Window;
	class Graphics
	{
	public:
        Graphics();								
        ~Graphics();

        void initialize(Window* window);

        bool bVSync;
        void clear(ID3D12PipelineState* pso);
        void present();

	protected:
        uint32_t unBackBufferCount;
        uint32_t unAntialiasing;   
        uint32_t unQuality;        
        float bgColor[4];

        ID3D12Device4* pDevice;             
        IDXGIFactory6* pFactory;
        IDXGISwapChain1* pSwapChain;
        uint32_t backBufferIndex;

        ID3D12CommandQueue* pCommandQueue;              
        ID3D12GraphicsCommandList* pCommandList;               
        ID3D12CommandAllocator* pCommandListAlloc;    

        ID3D12Resource** ppRenderTargets;
        ID3D12Resource* pDepthStencil; 
        ID3D12DescriptorHeap* pRenderTargetHeap; 
        ID3D12DescriptorHeap* pDepthStencilHeap;
        uint32_t rtDescriptorSize;      
        D3D12_VIEWPORT viewport;
        D3D12_RECT scissorRect;
                                
        ID3D12Fence* pFence;						
        uint64_t unCurrentFence;

        void logHardwareInfo();									 
        bool waitCommandQueue();								
        void submitCommands();
	};
}
