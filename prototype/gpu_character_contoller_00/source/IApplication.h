#pragma once

class IApplication
{
public:
   IApplication(
	const HWND hWnd, 
	const bool startFullScreen,
	const int defaultWidth,
	const int defaultHeight
	);
   virtual ~IApplication();

   virtual void Update();
   virtual void OnWindowMoved();
   virtual void OnWindowSizeChanged(const int width, const int height);
   virtual void OnActivated();
   virtual void OnDeactivated();
   virtual void OnSuspending();
   virtual void OnResuming();
   virtual void OnKey(const int vkCode, const int scanCode, const bool repeatFlag, const int repeatCount, bool upFlag);
   virtual void OnDestroy(const int exitCode);

   const int GetDefaultWidth() const { return m_defaultWidth; }
   const int GetDefaultHeight() const { return m_defaultHeight; }

   const bool GetInSizemove() const { return m_bInSizemove; }
   void SetInSizemove(const bool bInSizemove) { m_bInSizemove = bInSizemove; }

   const bool GetMinimized() const { return m_bMinimized; }
   void SetMinimized(const bool bMinimized) { m_bMinimized = bMinimized; }

   const bool GetFullScreen() const { return m_bFullScreen; }
   void SetFullScreen(const bool bFullScreen) { m_bFullScreen = bFullScreen; }


private:
   HWND m_hWnd; 
   int m_defaultWidth;
   int m_defaultHeight;

   bool m_bInSizemove;
   bool m_bInSuspend;
   bool m_bMinimized;
   bool m_bFullScreen;

};