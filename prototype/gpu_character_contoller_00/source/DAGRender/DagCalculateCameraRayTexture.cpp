#include "pch.h"
#include "DAGRender/DagCalculateCameraRayTexture.h"
#include "DrawSystemD12/DrawSystem.h"
#include "DrawSystemD12/Shader/Shader.h"
#include "DrawSystemD12/Geometry/GeometryGeneric.h"
#include "DrawSystemD12/CustomCommandList.h"
#include "FileSystem.h"

std::shared_ptr<DagCalculateCameraRayTexture> DagCalculateCameraRayTexture::Factory(
	DrawSystem& drawSystem
	)
{
	std::vector< D3D12_INPUT_ELEMENT_DESC > inputElementDescArray({
		D3D12_INPUT_ELEMENT_DESC{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0 // UINT InstanceDataStepRate;
			},
		D3D12_INPUT_ELEMENT_DESC{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0 // UINT InstanceDataStepRate;
			}
		});

	std::shared_ptr< Shader > pShader;
	std::shared_ptr< GeometryGeneric > pGeometry;

	auto pCommandList = drawSystem.CreateCustomCommandList();
	{
		pGeometry = drawSystem.MakeGeometryGeneric(
			pCommandList->GetCommandList(),
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			inputElementDescArray,
			std::vector<float>({
				 1.0f,  -1.0f,   1.0f,   1.0f,
				-1.0f,  -1.0f,   0.0f,   1.0f,
				 1.0f,   1.0f,   1.0f,   0.0f,
				-1.0f,   1.0f,   0.0f,   0.0f
				}),
			4);
	}

	{
		auto pVertexShaderData = FileSystem::SyncReadFile("CameraRayVertexShader.cso");
		auto pPixelShaderData = FileSystem::SyncReadFile("CameraRayPixelShader.cso");
		std::vector<DXGI_FORMAT> renderTargetFormat;
		renderTargetFormat.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);

		ShaderPipelineStateData shaderPipelineStateData(
			inputElementDescArray,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			DXGI_FORMAT_UNKNOWN, // DXGI_FORMAT_D32_FLOAT,
			renderTargetFormat,
			CD3DX12_BLEND_DESC(D3D12_DEFAULT),
			CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
			CD3DX12_DEPTH_STENCIL_DESC() //CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
			);

		pShader = drawSystem.MakeShader(
			pCommandList->GetCommandList(),
			shaderPipelineStateData,
			pVertexShaderData,
			nullptr,
			pPixelShaderData
			);
	}

	return std::make_shared<DagCalculateCameraRayTexture>(
		drawSystem,
		pShader,
		pGeometry
		);
}

DagCalculateCameraRayTexture::DagCalculateCameraRayTexture(
	DrawSystem& drawSystem,
	const std::shared_ptr< Shader >& pShader,
	const std::shared_ptr< GeometryGeneric >& pGeometry
	)
	: m_drawSystem(drawSystem)
	, m_pShader(pShader)
	, m_pGeometry(pGeometry)
	, m_pRenderTargetTexture()
{
	//nop
}

DagCalculateCameraRayTexture::~DagCalculateCameraRayTexture()
{
}

void DagCalculateCameraRayTexture::OnCalculate(
	std::shared_ptr< iDag2Value >& pValue,
	const std::vector< iDag2Value* >& arrayInputStack, 
	const std::vector< iDag2Value* >& arrayInputIndex
	)
{

}

