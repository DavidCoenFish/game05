#pragma once

#include "Common/DrawSystem/RenderTarget/IRenderTarget.h"
#include "Common/DrawSystem/RenderTarget/RenderTargetDepthData.h"
#include "Common/DrawSystem/RenderTarget/RenderTargetFormatData.h"

class DrawSystem;
class HeapWrapperItem;

class RenderTargetBackBuffer : public IRenderTarget
{
public:
	RenderTargetBackBuffer(
		DrawSystem* const pDrawSystem,
		ID3D12Device* const pDevice,
		const int bufferIndex,
		const RenderTargetFormatData& targetFormatData,
		const RenderTargetDepthData& targetDepthData,
		IDXGISwapChain* const pSwapChain,
		const int width,
		const int height
	);

private:
	virtual void StartRender(ID3D12GraphicsCommandList* const pCommandList) override;
	virtual void EndRender(ID3D12GraphicsCommandList* const pCommandList) override;

	virtual void GetFormatData(
		DXGI_FORMAT& depthFormat,
		int& renderTargetViewFormatCount,
		const DXGI_FORMAT*& pRenderTargetViewFormat
	) const override;

	virtual const int GetWidth() const override;
	virtual const int GetHeight() const override;

private:
	int m_bufferIndex;
	RenderTargetFormatData m_targetFormatData;
	D3D12_CLEAR_VALUE m_targetClearValue;
	RenderTargetDepthData m_targetDepthData;
	D3D12_CLEAR_VALUE m_depthClearValue;
	D3D12_VIEWPORT m_screenViewport;
	D3D12_RECT m_scissorRect;
	int m_backBufferWidth;
	int m_backBufferHeight;


	Microsoft::WRL::ComPtr<ID3D12Resource> m_pRenderTargetResource;
	std::shared_ptr<HeapWrapperItem> m_pRenderTargetDescriptor;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_pDepthResource;
	std::shared_ptr<HeapWrapperItem> m_pDepthStencilViewDescriptor;

	D3D12_RESOURCE_STATES m_currentState;

};
