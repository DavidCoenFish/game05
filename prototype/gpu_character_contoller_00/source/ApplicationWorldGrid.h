#pragma once

#include "IApplication.h"
class DrawSystem;
class Shader;
class GeometryGeneric;
class RenderTargetTexture;
class UnorderedAccess;

class ApplicationWorldGrid : public IApplication
{
   typedef IApplication BaseType;
public:
   static IApplication* const Factory(
		const HWND hWnd, 
		const bool bFullScreen,
		const int defaultWidth,
		const int defaultHeight
		);

	ApplicationWorldGrid(
		const HWND hWnd, 
		const bool bFullScreen,
		const int defaultWidth,
		const int defaultHeight
		);
	virtual ~ApplicationWorldGrid();

private:
	virtual void Update() override;
	virtual void OnWindowSizeChanged(const int width, const int height) override;
	virtual void OnDestroy(const int exitCode);

private:
	struct AWGConstantBuffer
	{
		float m_cameraRight[4];
		float m_cameraUp[4];
		float m_cameraAt[4];
		float m_cameraPos[4];
		float m_farHorizontalRadianVerticalRadians[4];
	};

	AWGConstantBuffer m_constantBuffer;


	std::unique_ptr< DrawSystem > m_pDrawSystem;

	struct DrawResources
	{
		std::shared_ptr< Shader > m_pPresentShader;
		std::shared_ptr< GeometryGeneric > m_pGeometry;
	};
	std::unique_ptr< DrawResources > m_pDrawResources;

};