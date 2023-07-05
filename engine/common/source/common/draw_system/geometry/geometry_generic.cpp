#include "CommonPCH.h"

#include "Common/DrawSystem/Geometry/GeometryGeneric.h"
#include "Common/DrawSystem/Geometry/IGeometry.h"

GeometryGeneric::GeometryGeneric(
   DrawSystem* const pDrawSystem,
   const D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
   const std::vector< D3D12_INPUT_ELEMENT_DESC >& inputElementDescArray,
   const std::vector< float >& vertexDataRaw,
   const int floatPerVertex
   )
   : IResource( pDrawSystem)
   , m_primitiveTopology(primitiveTopology)
   , m_inputElementDescArray(inputElementDescArray)
   , m_vertexRawData(vertexDataRaw)
   , m_floatPerVertex(floatPerVertex)
   , m_pVertexBuffer()
   , m_vertexBufferView{}
{
   //nop
}

void GeometryGeneric::Draw(ID3D12GraphicsCommandList* const pCommandList)
{
   IGeometry::DrawImplementation(
      pCommandList,
      (UINT)(m_vertexRawData.size() / m_floatPerVertex),
      m_primitiveTopology,
      m_vertexBufferView
      );
}
void GeometryGeneric::OnDeviceLost()
{
   IGeometry::DeviceLostImplementation(m_pVertexBuffer);
}

void GeometryGeneric::OnDeviceRestored(
   ID3D12GraphicsCommandList* const pCommandList,
   ID3D12Device2* const pDevice
   )
{
   IGeometry::DeviceRestoredImplementation(
      m_pDrawSystem,
      pCommandList,
      pDevice,
      (int)(m_vertexRawData.size() / m_floatPerVertex),
      sizeof(float) * m_floatPerVertex,
      m_pVertexBuffer,
      m_vertexBufferView,
      m_vertexRawData.data()
      );
}
