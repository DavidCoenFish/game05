#include "pch.h"
#include "DAGRender/DagCalculateCameraRayTexture.h"
#include "DAG2/Dag2Value.h"
#include "DAG2/Dag2ValueHelper.h"
#include "DrawSystemD12/CustomCommandList.h"
#include "DrawSystemD12/DrawSystem.h"
#include "DrawSystemD12/Geometry/GeometryGeneric.h"
#include "DrawSystemD12/RenderTarget/RenderTargetTexture.h"
#include "DrawSystemD12/Shader/Shader.h"
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

	const DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT; //DXGI_FORMAT_R32G32B32_FLOAT; //DXGI_FORMAT_R32G32B32A32_FLOAT;
	{
		auto pVertexShaderData = FileSystem::SyncReadFile("CameraRayVertexShader.cso");
		auto pPixelShaderData = FileSystem::SyncReadFile("CameraRayPixelShader.cso");
		std::vector<DXGI_FORMAT> renderTargetFormat;
		renderTargetFormat.push_back(format);

		ShaderPipelineStateData shaderPipelineStateData(
			inputElementDescArray,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			DXGI_FORMAT_UNKNOWN, // DXGI_FORMAT_D32_FLOAT,
			renderTargetFormat,
			CD3DX12_BLEND_DESC(D3D12_DEFAULT),
			CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
			CD3DX12_DEPTH_STENCIL_DESC() //CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
			);

		std::vector< std::shared_ptr< ConstantBufferInfo > > arrayShaderConstantsInfo;// = std::vector< std::shared_ptr< ConstantBufferInfo > >(),
		arrayShaderConstantsInfo.push_back(std::make_shared<ConstantBufferInfo>(
			std::vector<uint8_t>({0,0,0,0, 0,0,0,0}), 
			D3D12_SHADER_VISIBILITY_PIXEL
			));
		std::vector< std::shared_ptr< ShaderResourceInfo > > arrayShaderResourceInfo;

		pShader = drawSystem.MakeShader(
			pCommandList->GetCommandList(),
			shaderPipelineStateData,
			pVertexShaderData,
			nullptr,
			pPixelShaderData,
			arrayShaderResourceInfo,
			arrayShaderConstantsInfo
			);
	}

	return std::make_shared<DagCalculateCameraRayTexture>(
		drawSystem,
		format,
		pShader,
		pGeometry
		);
}

DagCalculateCameraRayTexture::DagCalculateCameraRayTexture(
	DrawSystem& drawSystem,
	const DXGI_FORMAT format,
	const std::shared_ptr< Shader >& pShader,
	const std::shared_ptr< GeometryGeneric >& pGeometry
	)
	: m_drawSystem(drawSystem)
	, m_format(format)
	, m_pShader(pShader)
	, m_pGeometry(pGeometry)
	, m_pRenderTargetTexture()
{
	//nop
}

DagCalculateCameraRayTexture::~DagCalculateCameraRayTexture()
{
}

struct LocalConstantBuffer
{
	float m_fovHorizontalVerticalRadian[2];
};

void DagCalculateCameraRayTexture::OnCalculate(
	std::shared_ptr< iDag2Value >& pValue,
	const std::vector< iDag2Value* >& arrayInputStack, 
	const std::vector< iDag2Value* >& arrayInputIndex
	)
{
	const int width = Dag2ValueHelper::Get<int>(arrayInputIndex[0], 0);
	const int height = Dag2ValueHelper::Get<int>(arrayInputIndex[1], 0);

	if ((0 == width) || (0 == height))
	{
		return;
	}

	const float fovhradian = Dag2ValueHelper::Get<float>(arrayInputIndex[2], 0.0f);

	auto pCommandList = m_drawSystem.CreateCustomCommandList();

	//make or resize the render target
	IRenderTarget* pIRenderTarget = m_pRenderTargetTexture.get();
	if (nullptr == m_pRenderTargetTexture)
	{
		std::vector< RenderTargetFormatData > targetFormatDataArray({
			RenderTargetFormatData(m_format, true, VectorFloat4(0.0f,0.0f,0.0f,0.0f))
			});
		RenderTargetDepthData targetDepthData;

		m_pRenderTargetTexture = m_drawSystem.MakeRenderTargetTexture(
			pCommandList->GetCommandList(),
			targetFormatDataArray,
			targetDepthData,
			width,
			height
			);
		pIRenderTarget = m_pRenderTargetTexture.get();
	}
	else if ((width != pIRenderTarget->GetWidth()) ||
		(height != pIRenderTarget->GetHeight()))
	{
		m_drawSystem.ResizeRenderTargetTexture(
			m_pRenderTargetTexture,
			pCommandList->GetCommandList(),
			width,
			height
			);
	}

	//render the texture
	pIRenderTarget->StartRender(pCommandList->GetCommandList());
	LocalConstantBuffer& constants = m_pShader->GetConstant<LocalConstantBuffer>(0);
	constants.m_fovHorizontalVerticalRadian[0] = fovhradian;
	const float fovvradian = fovhradian * ((float)height) / ((float)width);
	constants.m_fovHorizontalVerticalRadian[1] = fovvradian; 

	m_pShader->SetActivate(pCommandList->GetCommandList(), 0);
	m_pGeometry->Draw(pCommandList->GetCommandList());

	pIRenderTarget->EndRender(pCommandList->GetCommandList());

	Dag2ValueHelper::Assign(pValue, m_pRenderTargetTexture.get(), Dag2::None);
}

