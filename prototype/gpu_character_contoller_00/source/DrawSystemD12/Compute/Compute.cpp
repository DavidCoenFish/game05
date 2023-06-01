#include "pch.h"

#include "DrawSystemD12/Compute/Compute.h"

Compute::Compute(
	DrawSystem* const pDrawSystem,
	const std::shared_ptr< std::vector<uint8_t> >& pShaderData
	)
	: IResource(pDrawSystem)
	, m_pShaderData(pShaderData)
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

Compute::~Compute()
{
   //nop
}

void Compute::SetDebugName( const std::string& name )
{
   m_debugName = name;
}

void Compute::SetActivate(
   ID3D12GraphicsCommandList* const pCommandList,
   const int frameIndex
   )
{
   if (false == m_pipelineStateData.m_computeShader)
   {
      pCommandList->SetGraphicsRootSignature(m_rootSignature.Get());
   }
   else
   {
      pCommandList->SetComputeRootSignature(m_rootSignature.Get());
   }
   pCommandList->SetPipelineState(m_pipelineState.Get());

   int rootParamterIndex = 0;

   //b0,b1,b2,...
   for(const auto& iter : m_arrayConstantBuffer)
   {
      iter->Activate(pCommandList, rootParamterIndex, frameIndex);
      rootParamterIndex += 1;
   }

   //u0,u1,u2,...
   for(const auto& iter : m_arrayUnorderedAccessInfo)
   {
      iter->Activate(pCommandList, rootParamterIndex);
      rootParamterIndex += 1;
   }

   //t0,t1,t2,...
   for(const auto& iter : m_arrayShaderResourceInfo)
   {
      iter->Activate(pCommandList, rootParamterIndex);
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

void Compute::SetShaderResourceViewHandle( const int index, const std::shared_ptr< HeapWrapperItem >& pShaderResourceViewHandle )
{
   if ((0 <= index) && (index < (int)m_arrayShaderResourceInfo.size()))
   {
      m_arrayShaderResourceInfo[index]->SetShaderResourceViewHandle( pShaderResourceViewHandle );
   }
   return;
}

void Compute::SetUnorderedAccessViewHandle( const int index, const std::shared_ptr< HeapWrapperItem >& pUnorderedAccessViewHandle )
{
   if ((0 <= index) && (index < (int)m_arrayUnorderedAccessInfo.size()))
   {
      m_arrayUnorderedAccessInfo[index]->SetUnorderedAccessViewHandle( pUnorderedAccessViewHandle );
   }
   return;
}

void Compute::SetConstantBufferData( const int index, const std::vector<float>& data )
{
   if ((0 <= index) && (index < (int)m_arrayShaderConstantsInfo.size()))
   {
      auto& shaderConstantInfo = *m_arrayShaderConstantsInfo[index];
      const void* const pData = &data[0];
      shaderConstantInfo.UpdateData(pData, sizeof(float) * data.size());
   }
   return;
}

void Compute::OnDeviceLost()
{
   m_rootSignature.Reset();
   m_pipelineState.Reset();

   for( auto& pConstantBuffer : m_arrayConstantBuffer )
   {
      pConstantBuffer->DeviceLost();
   }
}

void Compute::OnDeviceRestored(
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
      m_pipelineState = MakePipelineStateComputeShader(
         pDevice,
         m_rootSignature,
         m_pComputeShaderData
      );
   }
   else
   {
      m_pipelineState = MakePipelineState(
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

