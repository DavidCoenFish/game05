#pragma once

#include "IApplication.h"
class DrawSystem;
class Shader;
class GeometryGeneric;
class RenderTargetTexture;
class UnorderedAccess;

class ApplicationCompute : public IApplication
{
   typedef IApplication BaseType;
public:
   static IApplication* const Factory(
		const HWND hWnd, 
		const bool bFullScreen,
		const int defaultWidth,
		const int defaultHeight
		);

	ApplicationCompute(
		const HWND hWnd, 
		const bool bFullScreen,
		const int defaultWidth,
		const int defaultHeight
		);
	virtual ~ApplicationCompute ();

private:
	virtual void Update() override;
	virtual void OnWindowSizeChanged(const int width, const int height) override;
	virtual void OnDestroy(const int exitCode);

private:
	struct ACConstantBuffer
	{
		float m_MaxThreadIter[4];
		float m_Window[4];
	};

	ACConstantBuffer m_constantBuffer;

	std::unique_ptr< DrawSystem > m_pDrawSystem;

	std::shared_ptr< Shader > m_pComputeShader;

	std::shared_ptr< UnorderedAccess > m_pComputeOutputTexture;
	std::shared_ptr< Shader > m_pPresentShader;
	std::shared_ptr< GeometryGeneric > m_pGeometry;

};