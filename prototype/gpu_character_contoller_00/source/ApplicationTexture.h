#pragma once

#include "IApplication.h"
class DrawSystem;
class Shader;
class ShaderResource;
class GeometryGeneric;

class ApplicationTexture : public IApplication
{
   typedef IApplication BaseType;
public:
   static IApplication* const Factory(
		const HWND hWnd, 
		const bool bFullScreen,
		const int defaultWidth,
		const int defaultHeight
		);

   ApplicationTexture (
		const HWND hWnd, 
		const bool bFullScreen,
		const int defaultWidth,
		const int defaultHeight
		);
   virtual ~ApplicationTexture ();

private:
	virtual void Update() override;
	virtual void OnWindowSizeChanged(const int width, const int height) override;
	virtual void OnDestroy(const int exitCode);

private:
	std::unique_ptr< DrawSystem > m_pDrawSystem;
	std::shared_ptr< Shader > m_pShader;
	std::shared_ptr< ShaderResource > m_pTexture;
	std::shared_ptr< GeometryGeneric > m_pGeometry;

};