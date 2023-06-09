#pragma once

#include "DAG2/Dag2NodeCalculateBase.h"
class DrawSystem;
class Shader;
class GeometryGeneric;
class RenderTargetTexture;

/*
input index
	0 width
	1 height
	2 fov width
output
	RenderTargetTexture*
*/
class DagCalculateCameraRayTexture : public Dag2NodeCalculateBase
{
public:
	static std::shared_ptr<DagCalculateCameraRayTexture> Factory(
		DrawSystem& drawSystem
		);

	DagCalculateCameraRayTexture(
		DrawSystem& drawSystem,
		const DXGI_FORMAT format,
		const std::shared_ptr< Shader >& pShader,
		const std::shared_ptr< GeometryGeneric >& pGeometry
		);
	~DagCalculateCameraRayTexture();

private:
	virtual void OnCalculate(
		std::shared_ptr< iDag2Value >& pValue,
		const std::vector< iDag2Value* >& arrayInputStack, 
		const std::vector< iDag2Value* >& arrayInputIndex
		) override;

private:
	DrawSystem& m_drawSystem;
	DXGI_FORMAT m_format;
	std::shared_ptr< Shader > m_pShader;
	std::shared_ptr< GeometryGeneric > m_pGeometry;
	std::shared_ptr< RenderTargetTexture > m_pRenderTargetTexture;

};