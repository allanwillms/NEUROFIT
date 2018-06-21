//***********************************************************************************************
//
//    Copyright (c) 1999 Axon Instruments.
//    All rights reserved.
//
//***********************************************************************************************
// HEADER:  TDrawWnd.HPP
// PURPOSE: Contains class definition for the generic CTDrawWin class.
// AUTHOR:  BHI  Feb 1999
//

#ifndef INC_TDRAWWND_HPP
#define INC_TDRAWWND_HPP

#include "AfxUtils.hpp"
#include "AfxMT.hpp"
#include "DisplaySurface.hpp"

class CTDrawWnd;

//====================================================================================================
// CLASS:   CDrawThread
// PURPOSE: Encapsulates a Win32 thread.
//
class CDrawThread
{
protected:
   CTDrawWnd        *m_pClient;     // Client object that does the drawing.
   BOOL              m_bNeeded;     // Set this flag FALSE to kill the thread.
   BOOL              m_bSuspend;    // Set this flag true to suspend the thread.
   HANDLE            m_hThread;     // The handle to the thread.
   CEvent            m_SuspendEvent;// Event that is signaled when the thread suspends.

private:    // Unimplemented member functions.
   CDrawThread(const CDrawThread &);
   const CDrawThread &operator=(const CDrawThread &);

public:
   // Construction/destruction
   CDrawThread();
   ~CDrawThread();
   
   // Create a thread in a suspended state..
   BOOL Create(CTDrawWnd *pClient);

   // Waits until the thread is required.
   BOOL WaitUntilNeeded();

   // Resume the thread from its initial slumber, or its suspended state.
   BOOL Resume();

   // Suspend the thread so that it does not chew up CPU cycles when not needed.
   BOOL Suspend();

   // Returns the status of the suspend flag.
   BOOL IsSuspended() const;

   // Terminate the thread.
   BOOL End();

   // Thread procedure used to drive the data pump.
   DWORD ThreadProc();
};

//====================================================================================================
// CLASS:   CTDrawWnd
// PURPOSE: Provides a generic window class class.
//
class CTDrawWnd
{
protected:
   HWND            m_hWnd;          // The window handle of the attached window
   HDC             m_hdcWin;        // The windows (own) device context.
   CRect           m_rClient;       // Size & position of the client area (maintained through OnSize).
   HFONT           m_hFont;         // The font to be used.
   CDisplaySurface m_Surface;       // The off-screen display surface.
   CDrawThread     m_Thread;        // The thread object.
   HPALETTE        m_hOldPalette;   // Save the original palette in the hDC.

private:    // Unimplemented member functions.
   CTDrawWnd(const CTDrawWnd &);
   const CTDrawWnd &operator=(const CTDrawWnd &);

protected:

   // Register the window class.
   virtual BOOL Register(HINSTANCE hInstance);
   virtual LPCSTR GetClassName();

   HDC  Lock();
   void Unlock();
   HDC  Relock();

   void SetBackgroundColor(COLORREF rgb);
   BOOL SelectPalette(BOOL bBackground);

public:
   // Construction/destruction
   CTDrawWnd();
   virtual ~CTDrawWnd();
   
   // Set/Get the associated window handle  (internal function called during creation).
   void SetHWND(HWND hWnd);
   HWND GetHWND();

   // Create the window.
   BOOL CreateWnd(LPCSTR pszTitle, DWORD dwStyle, LPCRECT pRect, 
                  HWND hwndParent, int nWindowID, HINSTANCE hInstance,
                  DWORD dwExStyle=0);

   // Destroy the window.
   BOOL DestroyWnd();
                  
   // Replace a place holder control.
   BOOL ReplaceControl(HWND hWnd, int nControlID);

   // Start the thread redrawing.
   void Redraw();

   // Message functions.   
   virtual BOOL  OnNCCreate(CREATESTRUCT *pCreateStruct);
   virtual BOOL  OnCreate(CREATESTRUCT *pCreateStruct);
   virtual void  OnSize();
   virtual void  SetText(LPCSTR pszText);
   virtual void  SetFont(HFONT hFont, BOOL fRedraw);
   virtual HFONT GetFont();
   virtual void  Draw();
   virtual void  OnPaint();
   virtual void  OnPaletteChanged(HWND hwndFocus);
   virtual BOOL  OnQueryNewPalette();
   virtual void  OnTimer();
   virtual void  OnClose();
   virtual void  OnDestroy();
   virtual LRESULT DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif    // INC_TDRAWWND_HPP
