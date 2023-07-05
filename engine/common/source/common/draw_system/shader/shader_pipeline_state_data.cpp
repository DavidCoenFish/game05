#include "CommonPCH.h"

#include "Common/DrawSystem/Shader/ShaderPipelineStateData.h"

ShaderPipelineStateData ShaderPipelineStateData::FactoryComputeShader()
{
   return ShaderPipelineStateData(
      std::vector< D3D12_INPUT_ELEMENT_DESC >(),
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,
      DXGI_FORMAT_UNKNOWN,
      std::vector<DXGI_FORMAT>(),
      CD3DX12_BLEND_DESC(D3D12_DEFAULT), //D3D12_DEFAULT),
      CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT), //D3D12_DEFAULT),
      CD3DX12_DEPTH_STENCIL_DESC(), //D3D12_DEFAULT)
      true
      );
}


ShaderPipelineStateData::ShaderPipelineStateData(
   const std::vector< D3D12_INPUT_ELEMENT_DESC >& inputElementDescArray,
   const D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType,
   const DXGI_FORMAT depthStencilViewFormat,
   const std::vector<DXGI_FORMAT>& renderTargetFormat,
   const D3D12_BLEND_DESC& blendState,
   const D3D12_RASTERIZER_DESC& rasterizerState,
   const D3D12_DEPTH_STENCIL_DESC& depthStencilState,
   const bool computeShader
   )
   : m_inputElementDescArray(inputElementDescArray)
   , m_primitiveTopologyType(primitiveTopologyType)
   , m_depthStencilViewFormat(depthStencilViewFormat)
   , m_renderTargetFormat(renderTargetFormat)
   , m_blendState(blendState)
   , m_rasterizerState(rasterizerState)
   , m_depthStencilState(depthStencilState)
   , m_computeShader(computeShader)
{
   return;
}
