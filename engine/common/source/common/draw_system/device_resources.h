#pragma once

#include "Common/DrawSystem/RenderTarget/RenderTargetFormatData.h"
#include "Common/DrawSystem/RenderTarget/RenderTargetDepthData.h"
//enum class DeviceResourcesOptions
//{
//   None = 0,
//   AllowTearing = 0x1,
//   EnableHDR = 0x2
//};
class DrawSystem;
class IResource;
class ScreenSizeResources;
class IRenderTarget;
struct RenderTargetFormatData;
struct RenderTargetDepthData;
namespace DirectX
{
	class GraphicsMemory;
	class GraphicsResource;
}

class DeviceResources
{
public:
	static const unsigned int c_AllowTearing = 0x1;
	static const unsigned int c_EnableHDR = 0x2;

	DeviceResources(
		const unsigned int backBufferCount,
		const D3D_FEATURE_LEVEL d3dFeatureLevel,
		const unsigned int options,
		const RenderTargetFormatData& targetFormatData,
		const RenderTargetDepthData& targetDepthData
	);
	~DeviceResources();
	void WaitForGpu() noexcept;

	const bool OnResize(
		DrawSystem* const pDrawSystem,
		const HWND hWnd,
		int& out_width,
		int& out_height
	);
	const int GetBackBufferIndex() const;
	DirectX::GraphicsResource AllocateUpload(const std::size_t size, void* const pDataOrNullptr, size_t alignment = 16);

	void Prepare(
		ID3D12GraphicsCommandList*& pCommandList
	);
	//void Clear();
	const bool Present();
	IRenderTarget* GetRenderTargetBackBuffer();

	ID3D12Device2* const GetD3dDevice();
	//ID3D12CommandQueue* const GetCommandQueue();

	ID3D12GraphicsCommandList* GetCustomCommandList();
	void CustomCommandListFinish(ID3D12GraphicsCommandList* pCommandList);

	//return true if size changed
	const bool CreateWindowSizeDependentResources(
		DrawSystem* const pDrawSystem,
		const HWND hWnd,
		int* out_pWidth = nullptr,
		int* out_pHeight = nullptr
	);

private:
	void GetAdapter(IDXGIAdapter1** ppAdapter, const D3D_FEATURE_LEVEL d3dFeatureLevel);
	void MoveToNextFrame();
	//void UpdateColorSpace();

	void WaitForCustomCommand();

private:
	unsigned int m_backBufferCount;

	unsigned int m_options;
	RenderTargetFormatData m_targetFormatData;
	RenderTargetDepthData m_targetDepthData;

	Microsoft::WRL::ComPtr<IDXGIFactory6> m_pDXGIFactory;
	DWORD m_dxgiFactoryFlags;
	Microsoft::WRL::ComPtr<ID3D12Device2> m_pDevice;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_pCommandQueue;

	UINT64 m_customCommandListFenceValue;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_pCustomCommandFence;
	Microsoft::WRL::Wrappers::Event m_customCommandFenceEvent;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_pCustomCommandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pCustomCommandList;

	std::unique_ptr< ScreenSizeResources > m_pScreenSizeResources;
	std::unique_ptr<DirectX::GraphicsMemory> m_pGraphicsMemory;

};
