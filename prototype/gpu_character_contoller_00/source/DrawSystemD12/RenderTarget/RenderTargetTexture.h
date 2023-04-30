#pragma once

#include "DrawSystemD12/RenderTarget/IRenderTarget.h"
#include "DrawSystemD12/IResource.h"
#include "DrawSystemD12/RenderTarget/RenderTargetDepthData.h"
#include "DrawSystemD12/RenderTarget/RenderTargetFormatData.h"

class DrawSystem;
class HeapWrapperItem;

class RenderTargetTexture : public IRenderTarget, public IResource
{
public:
	RenderTargetTexture(
		DrawSystem* const pDrawSystem,
		const std::vector< RenderTargetFormatData >& targetFormatDataArray,
		const RenderTargetDepthData& targetDepthData,
		const int width,
		const int height,
		const bool bResizeWithScreen = false
	);
	virtual ~RenderTargetTexture();

	std::shared_ptr< HeapWrapperItem > GetShaderResourceHeapWrapperItem(const int index = 0) const;
	std::shared_ptr< HeapWrapperItem > GetDepthResourceHeapWrapperItem() const;
	std::shared_ptr< HeapWrapperItem > GetDepthShaderResourceHeapWrapperItem() const;

	void Resize(
		ID3D12GraphicsCommandList* const pCommandList,
		ID3D12Device2* const pDevice,
		const int width,
		const int height
	);

private:
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored(
		ID3D12GraphicsCommandList* const pCommandList,
		ID3D12Device2* const pDevice
	) override;

	virtual void OnResize(
		ID3D12GraphicsCommandList* const pCommandList,
		ID3D12Device2* const pDevice,
		const int screenWidth,
		const int screenHeight
	) override;

	virtual void StartRender(ID3D12GraphicsCommandList* const pCommandList) override;
	virtual void EndRender(ID3D12GraphicsCommandList* const pCommandList) override;
	void TransitionResource(
		ID3D12GraphicsCommandList* const pCommandList,
		const D3D12_RESOURCE_STATES newStateRenderTarget,
		const D3D12_RESOURCE_STATES newStateDepthResource
	);

	//D3D12_RESOURCE_STATE_DEPTH_WRITE	= 0x10,
	//D3D12_RESOURCE_STATE_DEPTH_READ	= 0x20,

//virtual void GetPipelineState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc) const override;
	virtual void GetFormatData(
		DXGI_FORMAT& depthFormat,
		int& renderTargetViewFormatCount,
		const DXGI_FORMAT*& pRenderTargetViewFormat
	) const override;

	virtual const int GetWidth() const override;
	virtual const int GetHeight() const override;

private:
	int m_width;
	int m_height;
	bool m_bResizeWithScreen;

	std::vector< DXGI_FORMAT > m_targetFormatArray;

	struct Resource
	{
		explicit Resource(
			const D3D12_CLEAR_VALUE& clearValue,
			const DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
			const Microsoft::WRL::ComPtr<ID3D12Resource>& renderTarget = nullptr,
			const std::shared_ptr<HeapWrapperItem>& pRenderTargetViewDescriptor = nullptr,
			const std::shared_ptr<HeapWrapperItem>& pShaderResourceViewDescriptor = nullptr,
			const bool bClearColor = false,
			const bool bClearDepth = false,
			const bool bClearStencil = false
		);
		D3D12_CLEAR_VALUE m_clearValue;
		DXGI_FORMAT m_format;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTarget;
		std::shared_ptr<HeapWrapperItem> m_pRenderTargetViewDescriptor;
		std::shared_ptr<HeapWrapperItem> m_pShaderResourceViewDescriptor;
		bool m_bClearColor;
		bool m_bClearDepth;
		bool m_bClearStencil;
	};
	std::vector< std::shared_ptr< Resource > > m_targetResourceArray;
	std::vector< D3D12_CPU_DESCRIPTOR_HANDLE > m_arrayRenderTargetDescriptors; //cache the cpu handle of the target resource
	std::shared_ptr< Resource > m_pDepthResource;

	D3D12_VIEWPORT m_screenViewport;
	D3D12_RECT m_scissorRect;

	D3D12_RESOURCE_STATES m_currentStateRenderTarget;
	D3D12_RESOURCE_STATES m_currentStateDepthResource;


};
