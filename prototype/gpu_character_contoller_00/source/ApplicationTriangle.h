#pragma once

#include "IApplication.h"
class DrawSystem;
class Shader;
class GeometryGeneric;

class ApplicationTriangle : public IApplication
{
   typedef IApplication BaseType;
public:
   static IApplication* const Factory(
		const HWND hWnd, 
		const bool bFullScreen,
		const int defaultWidth,
		const int defaultHeight
		);

   ApplicationTriangle (
		const HWND hWnd, 
		const bool bFullScreen,
		const int defaultWidth,
		const int defaultHeight
		);
   virtual ~ApplicationTriangle ();

private:
	virtual void Update() override;
	virtual void OnWindowSizeChanged(const int width, const int height) override;
	virtual void OnDestroy(const int exitCode);

private:
	std::unique_ptr< DrawSystem > m_pDrawSystem;
	std::shared_ptr< Shader > m_pShader;
	std::shared_ptr< GeometryGeneric > m_pGeometry;

};