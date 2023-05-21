#pragma once

#include "IApplication.h"
class DrawSystem;
class Dag2Collection;
class Dag2NodeVariable;
class iDag2Node;

class ApplicationEmptyScene : public IApplication
{
   typedef IApplication BaseType;
public:
   static IApplication* const Factory(
		const HWND hWnd, 
		const bool bFullScreen,
		const int defaultWidth,
		const int defaultHeight
		);

   ApplicationEmptyScene (
		const HWND hWnd, 
		const bool bFullScreen,
		const int defaultWidth,
		const int defaultHeight
		);
   virtual ~ApplicationEmptyScene ();

private:
	virtual void Update() override;
	virtual void OnWindowSizeChanged(const int width, const int height) override;
	virtual void OnDestroy(const int exitCode);

private:
	std::unique_ptr< DrawSystem > m_pDrawSystem;
	std::unique_ptr< Dag2Collection > m_pDagCollection;

	std::shared_ptr< iDag2Node > m_pDagWidth;
	std::shared_ptr< iDag2Node > m_pDagHeight;
	std::shared_ptr< iDag2Node > m_pDagFovWidthRadian;  
	std::shared_ptr< iDag2Node > m_pDagCameraRenderTarget;

	std::shared_ptr< iDag2Node > m_pDagPresent;

};