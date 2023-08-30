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

        void Initialize(Window* window);

	protected:
        ID3D12Device4* pDevice;             
        IDXGIFactory6* pFactory;                   

        ID3D12CommandQueue* pCommandQueue;              
        ID3D12GraphicsCommandList* pCommandList;               
        ID3D12CommandAllocator* pCommandListAlloc;          

                                
        ID3D12Fence* pFence;						
        uint64_t unCurrentFence;

        void logHardwareInfo();									 
        bool waitCommandQueue();								
        void submitCommands();
	};
}
