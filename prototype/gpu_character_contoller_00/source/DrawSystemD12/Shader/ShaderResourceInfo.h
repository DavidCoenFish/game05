#pragma once

#include "DrawSystemD12/d3dx12.h"

class HeapWrapperItem;

struct ShaderResourceInfo
{
public:
	static std::shared_ptr< ShaderResourceInfo > FactorySampler(
		const std::shared_ptr< HeapWrapperItem >& pShaderResourceViewHandle,
		const D3D12_SHADER_VISIBILITY visiblity, //D3D12_SHADER_VISIBILITY_PIXEL
		const bool bMipMap = false
		);
	static std::shared_ptr< ShaderResourceInfo > FactoryDataSampler(
		const std::shared_ptr< HeapWrapperItem >& pShaderResourceViewHandle,
		const D3D12_SHADER_VISIBILITY visiblity
		);
	static std::shared_ptr< ShaderResourceInfo > FactoryNoSampler(
		const std::shared_ptr< HeapWrapperItem >& pShaderResourceViewHandle,
		const D3D12_SHADER_VISIBILITY visiblity
		);

	explicit ShaderResourceInfo(
		const std::shared_ptr< HeapWrapperItem >& pShaderResourceViewHandle = nullptr,
		const D3D12_STATIC_SAMPLER_DESC& staticSamplerDesc = CD3DX12_STATIC_SAMPLER_DESC(),
		const bool bUseSampler = true
		);

	void Activate( 
		ID3D12GraphicsCommandList* const pCommandList,
		const int rootParamIndex,
		const int frameIndex,
		const bool computeShader
		);
	//void ActivateSampler( 
	//   ID3D12GraphicsCommandList* const pCommandList,
	//   const int rootParamIndex
	//   );
	void SetShaderResourceViewHandle(const std::shared_ptr< HeapWrapperItem >& pShaderResourceViewHandle);
	const bool GetUseSampler() const { return m_bUseSampler; }
	const D3D12_STATIC_SAMPLER_DESC& GetStaticSamplerDesc() const { return m_staticSamplerDesc; }
	const D3D12_SHADER_VISIBILITY GetVisiblity() const;

private:
	bool m_bUseSampler;
	std::shared_ptr< HeapWrapperItem > m_pShaderResourceViewHandle;
	//std::shared_ptr< HeapWrapperItem > m_pSamplerHandle;
	D3D12_STATIC_SAMPLER_DESC m_staticSamplerDesc;

};
