#include "pch.h"

#include "DrawSystemD12/Shader/Shader.h"
#include "DrawSystemD12/Shader/ShaderResourceInfo.h"

#include "DrawSystemD12/DrawSystem.h"
#include "DrawSystemD12/IResource.h"
#include "DrawSystemD12/Shader/ConstantBuffer.h"
#include "DrawSystemD12/Shader/ConstantBufferInfo.h"
#include "DrawSystemD12/Shader/ShaderResourceInfo.h"
#include "DrawSystemD12/Shader/UnorderedAccessInfo.h"

std::shared_ptr< ConstantBuffer > MakeConstantBuffer(
	DrawSystem* const pDrawSystem,
	const std::shared_ptr< ConstantBufferInfo >& pConstantBufferInfo
	)
{
	if (nullptr == pConstantBufferInfo)
	{
		return nullptr;
	}
	const size_t constantBufferSize = pConstantBufferInfo->GetBufferSize();
	const void* const pData = pConstantBufferInfo->GetBufferData();
	const D3D12_SHADER_VISIBILITY visiblity = pConstantBufferInfo->GetVisiblity();
	const int frameCount = pDrawSystem->GetBackBufferCount();
	auto pConstantBuffer = std::make_shared< ConstantBuffer >( 
		frameCount, 
		constantBufferSize, 
		pDrawSystem->MakeHeapWrapperCbvSrvUav(),
		pData,
		visiblity
		);
	return pConstantBuffer;
}

static Microsoft::WRL::ComPtr<ID3D12RootSignature> MakeRootSignatureLocal(
	ID3D12Device* const pDevice,
	const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC& rootSignatureDesc
	)
{
	Microsoft::WRL::ComPtr<ID3D12RootSignature> pRootSignature;
	{
		Microsoft::WRL::ComPtr<ID3DBlob> signature;
		Microsoft::WRL::ComPtr<ID3DBlob> error;
		DX::ThrowIfFailed(
			D3D12SerializeVersionedRootSignature(
			&rootSignatureDesc, 
			&signature, 
			&error
			)
			);
		DX::ThrowIfFailed(
			pDevice->CreateRootSignature(
			0, 
			signature->GetBufferPointer(), 
			signature->GetBufferSize(),
			IID_PPV_ARGS(pRootSignature.ReleaseAndGetAddressOf())
			)
			);
	}
	static int s_trace = 0;
	const std::wstring name = (std::wstring(L"RootSignature:") + std::to_wstring(s_trace++)).c_str();
	pRootSignature->SetName(name.c_str());

	return pRootSignature;
}

static void RemoveDenyFlag(D3D12_ROOT_SIGNATURE_FLAGS& flag, const D3D12_SHADER_VISIBILITY visiblity)
{
	switch (visiblity)
	{
	default:
		break;
	case D3D12_SHADER_VISIBILITY_ALL:
		flag &= ~(
			D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS
			);
		break;
	case D3D12_SHADER_VISIBILITY_VERTEX:
		flag &= ~(
			D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS
			);
		break;
	case D3D12_SHADER_VISIBILITY_HULL:
		flag &= ~(
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
			);
		break;
	case D3D12_SHADER_VISIBILITY_DOMAIN:
		flag &= ~(
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
			);
		break;
	case D3D12_SHADER_VISIBILITY_GEOMETRY:
		flag &= ~(
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
			);
		break;
	case D3D12_SHADER_VISIBILITY_PIXEL:
		flag &= ~(
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS
			);
		break;
	case D3D12_SHADER_VISIBILITY_AMPLIFICATION:
		flag &= ~(
			D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS
			);
		break;
	case D3D12_SHADER_VISIBILITY_MESH:
		flag &= ~(
			D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS
			);
		break;
	}
	return;
}

//https://docs.microsoft.com/en-us/windows/win32/direct3d12/creating-a-root-signature
Microsoft::WRL::ComPtr<ID3D12RootSignature> MakeRootSignature(
	ID3D12Device* const pDevice,
	const std::vector< std::shared_ptr< ShaderResourceInfo > >& shaderTextureInfoArray,
	const std::vector< std::shared_ptr< ConstantBuffer > >& constantBufferArray,
	const std::vector< std::shared_ptr< UnorderedAccessInfo > >& arrayUnorderedAccessInfo
	)
{
	Microsoft::WRL::ComPtr<ID3D12RootSignature> pRootSignature;

	shaderTextureInfoArray;
	constantBufferArray;
	arrayUnorderedAccessInfo;

	D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;
	std::vector<D3D12_ROOT_PARAMETER1> rootParamterArray;
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplerDescArray;
	std::vector<std::shared_ptr< D3D12_DESCRIPTOR_RANGE1 >> descriptorRangeArray;

	//b0,b1,b2,...
	if (0 < constantBufferArray.size())
	{
		int offsetTableStart = 0;
		for(const auto& iter : constantBufferArray)
		{
			auto descriptorRange = std::make_shared< D3D12_DESCRIPTOR_RANGE1 >();
			descriptorRangeArray.push_back(descriptorRange);
			descriptorRange->BaseShaderRegister = 0;
			descriptorRange->Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
			descriptorRange->NumDescriptors = 1;
			descriptorRange->OffsetInDescriptorsFromTableStart = offsetTableStart;
			offsetTableStart += descriptorRange->NumDescriptors;
			descriptorRange->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

			D3D12_ROOT_PARAMETER1 param;
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = iter->GetVisiblity();
			param.DescriptorTable.NumDescriptorRanges = 1;
			param.DescriptorTable.pDescriptorRanges = descriptorRange.get();
			rootParamterArray.push_back(param);

			RemoveDenyFlag(flags, param.ShaderVisibility);
		}
	}

	//u0,u1,u2,...
	if (0 < arrayUnorderedAccessInfo.size())
	{
		int offsetTableStart = 0;
		for(const auto& iter : arrayUnorderedAccessInfo)
		{
			auto descriptorRange = std::make_shared< D3D12_DESCRIPTOR_RANGE1 >();
			descriptorRangeArray.push_back(descriptorRange);
			descriptorRange->BaseShaderRegister = 0;
			descriptorRange->Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
			descriptorRange->NumDescriptors = 1;
			descriptorRange->OffsetInDescriptorsFromTableStart = offsetTableStart;
			offsetTableStart += descriptorRange->NumDescriptors;
			descriptorRange->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

			D3D12_ROOT_PARAMETER1 param;
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = iter->GetVisiblity();
			param.DescriptorTable.NumDescriptorRanges = 1;
			param.DescriptorTable.pDescriptorRanges = descriptorRange.get();
			rootParamterArray.push_back(param);

			RemoveDenyFlag(flags, param.ShaderVisibility);
		}
	}

	//t0,t1,t2,...
	if (0 < shaderTextureInfoArray.size())
	{
		int offsetTableStart = 0;
		for(const auto& iter : shaderTextureInfoArray)
		{
			auto descriptorRange = std::make_shared< D3D12_DESCRIPTOR_RANGE1 >();
			descriptorRangeArray.push_back(descriptorRange);
			descriptorRange->BaseShaderRegister = 0;
			descriptorRange->Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
			descriptorRange->NumDescriptors = 1;
			descriptorRange->OffsetInDescriptorsFromTableStart = offsetTableStart;
			offsetTableStart += descriptorRange->NumDescriptors;
			descriptorRange->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

			D3D12_ROOT_PARAMETER1 param;
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = iter->GetVisiblity();
			param.DescriptorTable.NumDescriptorRanges = 1;
			param.DescriptorTable.pDescriptorRanges = descriptorRange.get();
			rootParamterArray.push_back(param);

			RemoveDenyFlag(flags, param.ShaderVisibility);
		}
	}

	//s0,s1,s2,...
	if (0 < shaderTextureInfoArray.size())
	{
#if 1
		for(const auto& iter : shaderTextureInfoArray)
		{
			if (false == iter->GetUseSampler())
			{
			continue;
			}
			staticSamplerDescArray.push_back(iter->GetStaticSamplerDesc());
		}
#else
		int offsetTableStart = 0;
		for(const auto& iter : shaderTextureInfoArray)
		{
			if (false == iter->GetUseSampler())
			{
			continue;
			}

			auto descriptorRange = std::make_shared< D3D12_DESCRIPTOR_RANGE1 >();
			descriptorRangeArray.push_back(descriptorRange);
			descriptorRange->BaseShaderRegister = 0;
			descriptorRange->Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
			descriptorRange->NumDescriptors = 1;
			descriptorRange->OffsetInDescriptorsFromTableStart = offsetTableStart;
			offsetTableStart += descriptorRange->NumDescriptors;
			descriptorRange->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;

			D3D12_ROOT_PARAMETER1 param;
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = iter->GetVisiblity();
			param.DescriptorTable.NumDescriptorRanges = 1;
			param.DescriptorTable.pDescriptorRanges = descriptorRange.get();
			rootParamterArray.push_back(param);

			RemoveDenyFlag(flags, param.ShaderVisibility);
		}
#endif
	}

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc(
		(UINT)rootParamterArray.size(),
		rootParamterArray.size() ? &rootParamterArray[0] : nullptr,
		(UINT)staticSamplerDescArray.size(),
		staticSamplerDescArray.size() ? &staticSamplerDescArray[0] : nullptr,
		flags
		);
	pRootSignature = MakeRootSignatureLocal(pDevice, rootSignatureDesc);

	return pRootSignature;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> MakeComputePipelineStateObject(
	ID3D12Device2* const pDevice,
	const Microsoft::WRL::ComPtr<ID3D12RootSignature>& pRootSignature,
	const std::shared_ptr< std::vector<uint8_t>>& pComputeShaderData
	)
{
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pComputePSO;

	// Create compute pipeline state
	D3D12_COMPUTE_PIPELINE_STATE_DESC descComputePSO = {};
	descComputePSO.pRootSignature = pRootSignature.Get();
	descComputePSO.CS.pShaderBytecode = pComputeShaderData->data();
	descComputePSO.CS.BytecodeLength = pComputeShaderData->size();

	DX::ThrowIfFailed( 
		pDevice->CreateComputePipelineState( 
			&descComputePSO, 
			IID_PPV_ARGS(pComputePSO.ReleaseAndGetAddressOf()) 
		) 
	);

	pComputePSO->SetName(L"ComputePSO");

	return pComputePSO;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> MakeGraphicsPipelineStateObject(
	ID3D12Device* const pDevice,
	const Microsoft::WRL::ComPtr<ID3D12RootSignature>& pRootSignature,
	const std::shared_ptr< std::vector<uint8_t>>& pVertexShaderData,
	const std::shared_ptr< std::vector<uint8_t>>& pGeometryShaderData,
	const std::shared_ptr< std::vector<uint8_t>>& pPixelShaderData,
	const ShaderPipelineStateData& pipelineStateData
	)
{
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pGraphicsPSO;
	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = pRootSignature.Get();
	if (nullptr != pVertexShaderData)
	{
		psoDesc.VS = { pVertexShaderData->data(), pVertexShaderData->size() };
	}
	if (nullptr != pPixelShaderData)
	{
		psoDesc.PS = { pPixelShaderData->data(), pPixelShaderData->size() };
	}
	//D3D12_SHADER_BYTECODE DS;
	//D3D12_SHADER_BYTECODE HS;
	if (nullptr != pGeometryShaderData)
	{
		psoDesc.GS = { pGeometryShaderData->data(), pGeometryShaderData->size() };
	}
	//D3D12_STREAM_OUTPUT_DESC StreamOutput;
	psoDesc.BlendState = pipelineStateData.m_blendState; //CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.RasterizerState = pipelineStateData.m_rasterizerState; //CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = pipelineStateData.m_depthStencilState;
	//psoDesc.DepthStencilState.DepthEnable = FALSE;
	//psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.InputLayout = { &pipelineStateData.m_inputElementDescArray[0], (UINT)pipelineStateData.m_inputElementDescArray.size() };
	//D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;
	psoDesc.PrimitiveTopologyType = pipelineStateData.m_primitiveTopologyType;
	psoDesc.NumRenderTargets = (UINT)pipelineStateData.m_renderTargetFormat.size(); // (UINT)numRenderTargets; //1
	for (int index = 0; index < (int)psoDesc.NumRenderTargets; ++index)
	{
		psoDesc.RTVFormats[index] = pipelineStateData.m_renderTargetFormat[index]; //DXGI_FORMAT_B8G8R8A8_UNORM;
	}
	psoDesc.DSVFormat = pipelineStateData.m_depthStencilViewFormat; //DXGI_FORMAT_UNKNOWN; // m_deviceResources->GetDepthBufferFormat();
	psoDesc.SampleDesc.Count = 1;
	//UINT NodeMask;
	//D3D12_CACHED_PIPELINE_STATE CachedPSO;
	//D3D12_PIPELINE_STATE_FLAGS Flags;

	DX::ThrowIfFailed(
		pDevice->CreateGraphicsPipelineState(
			&psoDesc,
			IID_PPV_ARGS(pGraphicsPSO.ReleaseAndGetAddressOf())
			)
		);

	pGraphicsPSO->SetName(L"GraphicsPSO");

	return pGraphicsPSO;
}

Shader::Shader(
	DrawSystem* const pDrawSystem,
	const ShaderPipelineStateData& pipelineStateData,
	const std::shared_ptr< std::vector<uint8_t> >& pVertexShaderData,
	const std::shared_ptr< std::vector<uint8_t> >& pGeometryShaderData,
	const std::shared_ptr< std::vector<uint8_t> >& pPixelShaderData,
	const std::vector< std::shared_ptr< ShaderResourceInfo > >& arrayShaderResourceInfo,
	const std::vector< std::shared_ptr< ConstantBufferInfo > >& arrayShaderConstantsInfo,
	const std::shared_ptr< std::vector<uint8_t> >& pComputeShaderData,
	const std::vector< std::shared_ptr< UnorderedAccessInfo > >& arrayUnorderedAccessInfo
	)
	: IResource(pDrawSystem)
	, m_pipelineStateData(pipelineStateData)
	, m_pVertexShaderData(pVertexShaderData)
	, m_pGeometryShaderData(pGeometryShaderData)
	, m_pPixelShaderData(pPixelShaderData)
	, m_arrayShaderResourceInfo(arrayShaderResourceInfo)
	, m_arrayShaderConstantsInfo(arrayShaderConstantsInfo)
	, m_pComputeShaderData(pComputeShaderData)
	, m_arrayUnorderedAccessInfo(arrayUnorderedAccessInfo)
{
	for (auto& iter : m_arrayShaderConstantsInfo)
	{
		auto pResult = MakeConstantBuffer(pDrawSystem, iter);
		if (pResult)
		{
			m_arrayConstantBuffer.push_back(pResult);
		}
	}

	return;
}

Shader::~Shader()
{
	//nop
}

void Shader::SetDebugName( const std::string& name )
{
	m_debugName = name;
}

void Shader::SetActivate(
	ID3D12GraphicsCommandList* const pCommandList,
	const int frameIndex
	)
{
	if (true == m_pipelineStateData.m_computeShader)
	{
		pCommandList->SetComputeRootSignature(m_rootSignature.Get());
	}
	else
	{
		pCommandList->SetGraphicsRootSignature(m_rootSignature.Get());
	}
	pCommandList->SetPipelineState(m_pipelineStateObject.Get());

	int rootParamterIndex = 0;

	//b0,b1,b2,...
	for(const auto& iter : m_arrayConstantBuffer)
	{
		iter->Activate(pCommandList, rootParamterIndex, frameIndex, m_pipelineStateData.m_computeShader);
		rootParamterIndex += 1;
	}

	//u0,u1,u2,...
	for(const auto& iter : m_arrayUnorderedAccessInfo)
	{
		iter->Activate(pCommandList, rootParamterIndex, frameIndex, m_pipelineStateData.m_computeShader);
		rootParamterIndex += 1;
	}

	//t0,t1,t2,...
	for(const auto& iter : m_arrayShaderResourceInfo)
	{
		iter->Activate(pCommandList, rootParamterIndex, frameIndex, m_pipelineStateData.m_computeShader);
		rootParamterIndex += 1;
	}

	//s0,s1,s2,...
	//for(const auto& iter : m_arrayShaderResourceInfo)
	//{
	//   if (false == iter->GetUseSampler())
	//   {
	//      continue;
	//   }
	//   iter->ActivateSampler(pCommandList, rootParamterIndex);
	//   rootParamterIndex += 1;
	//}
}


//void Shader::Dispatch(
//	ID3D12GraphicsCommandList* const pCommandList,
//	const int threadGroupX,
//	const int threadGroupY,
//	const int threadGroupZ
//	)
//{
//	pCommandList->Dispatch(threadGroupX, threadGroupY, threadGroupZ);
//}

void Shader::SetShaderResourceViewHandle( const int index, const std::shared_ptr< HeapWrapperItem >& pShaderResourceViewHandle )
{
	if ((0 <= index) && (index < (int)m_arrayShaderResourceInfo.size()))
	{
		m_arrayShaderResourceInfo[index]->SetShaderResourceViewHandle( pShaderResourceViewHandle );
	}
	return;
}

void Shader::SetUnorderedAccessViewHandle( const int index, const std::shared_ptr< HeapWrapperItem >& pUnorderedAccessViewHandle )
{
	if ((0 <= index) && (index < (int)m_arrayUnorderedAccessInfo.size()))
	{
		m_arrayUnorderedAccessInfo[index]->SetUnorderedAccessViewHandle( pUnorderedAccessViewHandle );
	}
	return;
}

void Shader::SetConstantBufferData( const int index, const std::vector<float>& data )
{
	if ((0 <= index) && (index < (int)m_arrayShaderConstantsInfo.size()))
	{
		auto& shaderConstantInfo = *m_arrayShaderConstantsInfo[index];
		const void* const pData = &data[0];
		shaderConstantInfo.UpdateData(pData, sizeof(float) * data.size());
	}
	return;
}

void Shader::OnDeviceLost()
{
	m_rootSignature.Reset();
	m_pipelineStateObject.Reset();

	for( auto& pConstantBuffer : m_arrayConstantBuffer )
	{
		pConstantBuffer->DeviceLost();
	}
}

void Shader::OnDeviceRestored(
	ID3D12GraphicsCommandList* const,
	ID3D12Device2* const pDevice
	)
{
	for( auto& pConstantBuffer : m_arrayConstantBuffer )
	{
		pConstantBuffer->DeviceRestored(pDevice);
	}
	m_rootSignature = MakeRootSignature(
		pDevice,
		m_arrayShaderResourceInfo,
		m_arrayConstantBuffer,
		m_arrayUnorderedAccessInfo
		);
	if (true == m_pipelineStateData.m_computeShader)
	{
		m_pipelineStateObject = MakeComputePipelineStateObject(
			pDevice,
			m_rootSignature,
			m_pComputeShaderData
		);
	}
	else
	{
		m_pipelineStateObject = MakeGraphicsPipelineStateObject(
			pDevice,
			m_rootSignature,
			m_pVertexShaderData,
			m_pGeometryShaderData,
			m_pPixelShaderData,
			m_pipelineStateData
			);
	}

	return;

}

