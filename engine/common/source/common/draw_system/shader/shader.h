#pragma once

#include "Common/DrawSystem/IResource.h"
#include "Common/DrawSystem/Shader/ShaderPipelineStateData.h"
#include "Common/DrawSystem/Shader/ConstantBufferInfo.h"

struct ConstantBufferInfo;
struct ShaderResourceInfo;
struct ConstantBuffer;
struct UnorderedAccessInfo;
class HeapWrapperItem;

class Shader : public IResource
{
public:
   Shader(
      DrawSystem* const pDrawSystem,
      const ShaderPipelineStateData& pipelineStateData,
      const std::shared_ptr< std::vector<uint8_t> >& pVertexShaderData,
      const std::shared_ptr< std::vector<uint8_t> >& pGeometryShaderData,
      const std::shared_ptr< std::vector<uint8_t> >& pPixelShaderData,
      const std::vector< std::shared_ptr< ShaderResourceInfo > >& arrayShaderResourceInfo = std::vector< std::shared_ptr< ShaderResourceInfo > >(),
      const std::vector< std::shared_ptr< ConstantBufferInfo > >& arrayShaderConstantsInfo = std::vector< std::shared_ptr< ConstantBufferInfo > >(),
      const std::shared_ptr< std::vector<uint8_t> >& pComputeShaderData = std::shared_ptr< std::vector<uint8_t> >(),
      const std::vector< std::shared_ptr< UnorderedAccessInfo > >& arrayUnorderedAccessInfo = std::vector< std::shared_ptr< UnorderedAccessInfo > >()
      );
   virtual ~Shader();

   void SetDebugName( const std::string& name );

   void SetActivate(
      ID3D12GraphicsCommandList* const pCommandList,
      const int frameIndex
      );

   void SetShaderResourceViewHandle( const int index, const std::shared_ptr< HeapWrapperItem >& pShaderResourceViewHandle );
   void SetUnorderedAccessViewHandle( const int index, const std::shared_ptr< HeapWrapperItem >& pUnorderedAccessViewHandle );
   void SetConstantBufferData( const int index, const std::vector<float>& data );

   template <typename CONSTANTS>
   CONSTANTS& GetConstant( const int index)
   {
      if ((0 <= index) && (index < m_arrayShaderConstantsInfo.size()))
      {
         auto& shaderConstantInfo = *m_arrayShaderConstantsInfo[index];
         assert(sizeof(CONSTANTS) == shaderConstantInfo.GetBufferSize());
         const void* const pData = shaderConstantInfo.GetBufferData();
         return (*((CONSTANTS*)pData));
      }
      static CONSTANTS result;
      return result;
   }

private:
   virtual void OnDeviceLost() override;

   virtual void OnDeviceRestored(
      ID3D12GraphicsCommandList* const,
      ID3D12Device2* const pDevice
      ) override;

private:
   ShaderPipelineStateData m_pipelineStateData;
   std::shared_ptr< std::vector<uint8_t> > m_pVertexShaderData;
   std::shared_ptr< std::vector<uint8_t> > m_pGeometryShaderData;
   std::shared_ptr< std::vector<uint8_t> > m_pPixelShaderData;
   std::vector< std::shared_ptr< ShaderResourceInfo > > m_arrayShaderResourceInfo;
   std::vector< std::shared_ptr< ConstantBufferInfo > > m_arrayShaderConstantsInfo;
   std::shared_ptr< std::vector<uint8_t> > m_pComputeShaderData;
   std::vector< std::shared_ptr< UnorderedAccessInfo > > m_arrayUnorderedAccessInfo;

   std::vector< std::shared_ptr< ConstantBuffer > > m_arrayConstantBuffer;
   Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
   Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

   std::string m_debugName;
};
