#pragma once

#include "Common/DrawSystem/RenderTarget/RenderTargetFormatData.h"
#include "Common/DrawSystem/RenderTarget/RenderTargetDepthData.h"
#include "Common/DrawSystem/Geometry/Geometry.h"
#include "Common/DrawSystem/DeviceResources.h"

class DeviceResources;
class DrawSystemFrame;
class IResource;
class IRenderTarget; //* GetRenderTargetBackBuffer();
class HeapWrapper;
class HeapWrapperItem;
class CustomCommandList;
class Shader;
struct ShaderPipelineStateData;
struct ConstantBufferInfo;
struct ShaderResourceInfo;
struct ConstantBuffer;
struct UnorderedAccessInfo;
class GeometryGeneric;
class ShaderResource;
class RenderTargetTexture;
class JSONDrawSystem;
class UnorderedAccess;

namespace DirectX
{
   class GraphicsMemory;
   class GraphicsResource;
};

class DrawSystem
{
public:
   static std::unique_ptr< DrawSystem> Factory(const HWND hWnd, const JSONDrawSystem& json);

   DrawSystem(
      const HWND hWnd,
      const unsigned int backBufferCount = 2,
      const D3D_FEATURE_LEVEL d3dFeatureLevel = D3D_FEATURE_LEVEL_11_0,
      const unsigned int options = 0,
      const RenderTargetFormatData& targetFormatData = RenderTargetFormatData(DXGI_FORMAT_B8G8R8A8_UNORM),
      const RenderTargetDepthData& targetDepthData = RenderTargetDepthData()
      );
   ~DrawSystem();
   void WaitForGpu() noexcept;
   void OnResize();

   void AddResource(IResource* const pResource);
   void RemoveResource(IResource* const pResource);
   const int GetBackBufferIndex() const;
   const int GetBackBufferCount() const { return m_backBufferCount; }

   DirectX::GraphicsResource AllocateConstant(const std::size_t size, void* const pConstants);
   DirectX::GraphicsResource AllocateUpload(
      const std::size_t size, 
      void* const pDataOrNullptr = nullptr, 
      size_t alignment = 16
      );

   std::shared_ptr< Shader > MakeShader(
      ID3D12GraphicsCommandList* const pCommandList,
      const ShaderPipelineStateData& pipelineStateData,
      const std::shared_ptr< std::vector<uint8_t> >& pVertexShaderData,
      const std::shared_ptr< std::vector<uint8_t> >& pGeometryShaderData,
      const std::shared_ptr< std::vector<uint8_t> >& pPixelShaderData,
      const std::vector< std::shared_ptr< ShaderResourceInfo > >& arrayShaderResourceInfo = std::vector< std::shared_ptr< ShaderResourceInfo > >(),
      const std::vector< std::shared_ptr< ConstantBufferInfo > >& arrayShaderConstantsInfo = std::vector< std::shared_ptr< ConstantBufferInfo > >(),
      const std::shared_ptr< std::vector<uint8_t> >& pComputeShaderData = std::shared_ptr< std::vector<uint8_t> >(),
      const std::vector< std::shared_ptr< UnorderedAccessInfo > >& arrayUnorderedAccessInfo = std::vector< std::shared_ptr< UnorderedAccessInfo > >()
   );

   std::shared_ptr< GeometryGeneric > MakeGeometryGeneric(
      ID3D12GraphicsCommandList* const pCommandList,
      const D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
      const std::vector< D3D12_INPUT_ELEMENT_DESC >& inputElementDescArray,
      const std::vector< float >& vertexDataRaw,
      const int floatPerVertex
      );

   //was moving away from Geometry and towards GeometryGeneric as for json it is easier to load generic float data
   template <typename TypeVertex >
   std::shared_ptr< Geometry<TypeVertex> > MakeGeometry(
      ID3D12GraphicsCommandList* const pCommandList,
      const D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
      const std::vector< D3D12_INPUT_ELEMENT_DESC >& inputElementDescArray,
      const std::vector< TypeVertex >& vertexData
      )
   {
      auto pResult = std::make_shared<Geometry<TypeVertex>>(
         this,
         primitiveTopology,
         inputElementDescArray,
         vertexData
         );
      if (pResult && m_pDeviceResources)
      {
         ((IResource*)(pResult.get()))->OnDeviceRestored(
            pCommandList,
            m_pDeviceResources->GetD3dDevice()
            );
      }
      return pResult;
   }

   std::shared_ptr< ShaderResource > MakeShaderResource(
      ID3D12GraphicsCommandList* const pCommandList,
      const std::shared_ptr< HeapWrapperItem >& pHeapWrapperItem,
      const D3D12_RESOURCE_DESC& desc, 
      const D3D12_SHADER_RESOURCE_VIEW_DESC& shaderResourceViewDesc,
      const std::vector<uint8_t>& data = std::vector<uint8_t>()
      );

   std::shared_ptr< UnorderedAccess > MakeUnorderedAccess(
      ID3D12GraphicsCommandList* const pCommandList,
      const std::shared_ptr< HeapWrapperItem >& pHeapWrapperItem,
      const D3D12_RESOURCE_DESC& desc, 
      const D3D12_UNORDERED_ACCESS_VIEW_DESC& unorderedAccessViewDesc
      );

   std::shared_ptr< RenderTargetTexture > MakeRenderTargetTexture(
      ID3D12GraphicsCommandList* const pCommandList,
      const std::vector< RenderTargetFormatData >& targetFormatDataArray,
      const RenderTargetDepthData& targetDepthData,
      const int width,
      const int height,
      const bool bResizeWithScreen = false
      );

   std::shared_ptr<CustomCommandList> CreateCustomCommandList();
   void CustomCommandListFinish(ID3D12GraphicsCommandList* pCommandList);

   std::unique_ptr< DrawSystemFrame > CreateNewFrame(); //ctor == Prepare, dtor == Present

   void Prepare(ID3D12GraphicsCommandList*& pCommandList);
   //void Clear();
   void Present();
   IRenderTarget* GetRenderTargetBackBuffer();

   std::shared_ptr<HeapWrapperItem> MakeHeapWrapperCbvSrvUav(const int length = 1);
   std::shared_ptr<HeapWrapperItem> MakeHeapWrapperSampler(const int length = 1);
   std::shared_ptr<HeapWrapperItem> MakeHeapWrapperRenderTargetView(const int length = 1);
   std::shared_ptr<HeapWrapperItem> MakeHeapWrapperDepthStencilView(const int length = 1);

private:
   void CreateDeviceResources();

private:
   HWND m_hWnd;
   unsigned int m_backBufferCount;
   D3D_FEATURE_LEVEL m_d3dFeatureLevel;
   unsigned int m_options;
   RenderTargetFormatData m_targetFormatData;
   RenderTargetDepthData m_targetDepthData;

   std::unique_ptr< DeviceResources > m_pDeviceResources;

   std::list< IResource* > m_listResource;

   std::shared_ptr< HeapWrapper > m_pHeapWrapperCbvSrvUav;
   std::shared_ptr< HeapWrapper > m_pHeapWrapperSampler;
   std::shared_ptr< HeapWrapper > m_pHeapWrapperRenderTargetView;
   std::shared_ptr< HeapWrapper > m_pHeapWrapperDepthStencilView;

};
