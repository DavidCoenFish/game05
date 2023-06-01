#include "pch.h"

#include "DrawSystemD12/Shader/ShaderResourceInfo.h"
#include "DrawSystemD12/HeapWrapper/HeapWrapperItem.h"

std::shared_ptr< ShaderResourceInfo > ShaderResourceInfo::FactorySampler(
	const std::shared_ptr< HeapWrapperItem >& pShaderResourceViewHandle,
	const D3D12_SHADER_VISIBILITY visiblity,
	const bool bMipMap
	)
{
	D3D12_STATIC_SAMPLER_DESC staticSamplerDesc   {
		bMipMap ? D3D12_FILTER_ANISOTROPIC : D3D12_FILTER_MIN_MAG_MIP_LINEAR, //D3D12_FILTER Filter;
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressU;
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressV;
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressW;
		0, //FLOAT MipLODBias;
		16, //UINT MaxAnisotropy;
		D3D12_COMPARISON_FUNC_LESS_EQUAL, //D3D12_COMPARISON_FUNC ComparisonFunc;
		D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, //D3D12_STATIC_BORDER_COLOR BorderColor;
		0, //FLOAT MinLOD;
		bMipMap ? D3D12_FLOAT32_MAX : 0, //FLOAT MaxLOD;
		0, //UINT ShaderRegister;
		0, //UINT RegisterSpace;
		visiblity      //D3D12_SHADER_VISIBILITY_PIXEL //D3D12_SHADER_VISIBILITY ShaderVisibility;
	};

	return std::make_shared< ShaderResourceInfo >(
		pShaderResourceViewHandle,
		staticSamplerDesc
		);
}

std::shared_ptr< ShaderResourceInfo > ShaderResourceInfo::FactoryDataSampler(
	const std::shared_ptr< HeapWrapperItem >& pShaderResourceViewHandle,
	const D3D12_SHADER_VISIBILITY visiblity
	)
{
	D3D12_STATIC_SAMPLER_DESC staticSamplerDesc   {
		D3D12_FILTER_MIN_MAG_MIP_POINT, //D3D12_FILTER Filter;
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressU;
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressV;
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressW;
		0, //FLOAT MipLODBias;
		16, //UINT MaxAnisotropy;
		D3D12_COMPARISON_FUNC_LESS_EQUAL, //D3D12_COMPARISON_FUNC ComparisonFunc;
		D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, //D3D12_STATIC_BORDER_COLOR BorderColor;
		0, //FLOAT MinLOD;
		0, //FLOAT MaxLOD;
		0, //UINT ShaderRegister;
		0, //UINT RegisterSpace;
		visiblity      //D3D12_SHADER_VISIBILITY_PIXEL //D3D12_SHADER_VISIBILITY ShaderVisibility;
	};
	return std::make_shared< ShaderResourceInfo >(
		pShaderResourceViewHandle,
		staticSamplerDesc
		);
}

std::shared_ptr< ShaderResourceInfo > ShaderResourceInfo::FactoryNoSampler(
	const std::shared_ptr< HeapWrapperItem >& pShaderResourceViewHandle,
	const D3D12_SHADER_VISIBILITY visiblity
	)
{
	D3D12_STATIC_SAMPLER_DESC staticSamplerDesc   {};
	staticSamplerDesc.ShaderVisibility = visiblity;
	return std::make_shared< ShaderResourceInfo >(
		pShaderResourceViewHandle,
		staticSamplerDesc,
		false
		);
}

ShaderResourceInfo::ShaderResourceInfo(
	const std::shared_ptr< HeapWrapperItem >& pShaderResourceViewHandle,
	const D3D12_STATIC_SAMPLER_DESC& staticSamplerDesc,
	const bool bUseSampler
	)
	: m_pShaderResourceViewHandle(pShaderResourceViewHandle)
	, m_staticSamplerDesc(staticSamplerDesc)
	, m_bUseSampler(bUseSampler)
{
	return;
}

void ShaderResourceInfo::Activate( 
	ID3D12GraphicsCommandList* const pCommandList,
	const int rootParamIndex,
	const int frameIndex,
	const bool computeShader
	)
{
	if (m_pShaderResourceViewHandle)
	{
		auto heap = m_pShaderResourceViewHandle->GetHeap();
		pCommandList->SetDescriptorHeaps(1, &heap);
		if (true == computeShader)
		{
			pCommandList->SetComputeRootDescriptorTable(
				rootParamIndex, 
				m_pShaderResourceViewHandle->GetGPUHandleFrame(frameIndex)
				);
		}
		else
		{
			pCommandList->SetGraphicsRootDescriptorTable(
				rootParamIndex, 
				m_pShaderResourceViewHandle->GetGPUHandleFrame(frameIndex)
				);
		}
	}

	return;
}

void ShaderResourceInfo::SetShaderResourceViewHandle(const std::shared_ptr< HeapWrapperItem >& pShaderResourceViewHandle)
{
	m_pShaderResourceViewHandle = pShaderResourceViewHandle;
	return;
}


const D3D12_SHADER_VISIBILITY ShaderResourceInfo::GetVisiblity() const
{
	return m_staticSamplerDesc.ShaderVisibility;
}
