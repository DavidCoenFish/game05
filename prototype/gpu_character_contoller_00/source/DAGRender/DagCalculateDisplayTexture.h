#pragma once

#include "DAG2/Dag2NodeCalculateBase.h"
class DrawSystem;
class Shader;
class GeometryGeneric;

/*
input index
	texture

do we really need to derrive off Dag2NodeCalculate? yes, we own a shader and geometry
*/
class DagCalculateDisplayTexture : public Dag2NodeCalculateBase
{
public:
	static std::shared_ptr<DagCalculateDisplayTexture> Factory(
		DrawSystem& drawSystem
		);

	DagCalculateDisplayTexture(
		const std::shared_ptr< Shader >& pShader,
		const std::shared_ptr< GeometryGeneric >& pGeometry
		);
	~DagCalculateDisplayTexture();

private:
	virtual void OnCalculate(
		std::shared_ptr< iDag2Value >& pValue,
		const std::vector< iDag2Value* >& arrayInputStack, 
		const std::vector< iDag2Value* >& arrayInputIndex
		) override;

private:
	std::shared_ptr< Shader > m_pShader;
	std::shared_ptr< GeometryGeneric > m_pGeometry;

};