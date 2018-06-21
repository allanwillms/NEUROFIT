//***********************************************************************************************
//
//    Copyright (c) 1999 Axon Instruments.
//    All rights reserved.
//
//***********************************************************************************************
// HEADER:  TDrawWnd.CPP
// PURPOSE: Generic Threaded background drawing Window Class. 
// AUTHOR:  BHI  Feb 1999
//
#include "wincpp.hpp"
#include <process.h>

#include "TDrawWnd.hpp"
#include "\AxonDev\Comp\Axoutils32\Axoutils32.h"

//===============================================================================================
// Macros for setting and getting the pointer to the acquisition object.
#define GWL_TDRAWWND 0      // Offset to CTDrawWnd pointer in Wnd extra bytes.
#define GETOBJPTR(hWnd)       ((CTDrawWnd *)GetWindowLong((hWnd), GWL_TDRAWWND))
#define SETOBJPTR(hWnd, pObj) (SetWindowLong((hWnd), GWL_TDRAWWND, long(pObj)))

// Screen refresh timer constants.
const UINT c_uTimerID     = 42;
const UINT c_uTimerPeriod = 200;

// Something to throw to abort drawing.
const int  c_nINTERRUPT_EXCEPTION = 666;

//===============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Sets the m_hWnd variable and initializes the object.
//
CTDrawWnd::CTDrawWnd()
{
   MEMBERASSERT();
   m_hWnd        = NULL;
   m_hdcWin      = NULL;
   m_hFont       = NULL;
   m_hOldPalette = NULL;
}

//===============================================================================================
// FUNCTION: Destructor
// PURPOSE:  Cleans up on destruction of the object.
//
CTDrawWnd::~CTDrawWnd()
{
   MEMBERASSERT();
   
   // Destroy the window if it exists (benign if it does not).
   DestroyWnd();
}

//===============================================================================================
// FUNCTION: SetHWND
// PURPOSE:  Set associated window handle.
//
void CTDrawWnd::SetHWND(HWND hWnd)
{
   MEMBERASSERT();
   m_hWnd = hWnd;
}

//===============================================================================================
// FUNCTION: GetHWND
// PURPOSE:  Get the HWND associated with the object.
//
HWND CTDrawWnd::GetHWND()
{
   MEMBERASSERT();
   return m_hWnd;
}

//===============================================================================================
// FUNCTION: GetClassName
// PURPOSE:  Return a pointer to the name of the window class.
// NOTES:    If a derived class overrides the Register() member function it must also have its
//           own class static flag to indicate whether registration has been completed ok, and
//           a unique class name provided by an overridden GetClassName() function.
//
LPCSTR CTDrawWnd::GetClassName()
{
   MEMBERASSERT();
   return "AXO_CTDrawWndClass32";
}

//===============================================================================================
// FUNCTION: OnNCCreate
// PURPOSE:  Processes the WM_NCCREATE message.
//
BOOL CTDrawWnd::OnNCCreate(LPCREATESTRUCT lpCreateStruct)
{
   MEMBERASSERT();
   return (BOOL)::DefWindowProc(m_hWnd, WM_NCCREATE, 0, LPARAM(lpCreateStruct));
}

//===============================================================================================
// FUNCTION: OnCreate
// PURPOSE:  Processes the WM_CREATE message.
//
BOOL CTDrawWnd::OnCreate(CREATESTRUCT FAR* lpCreateStruct)
{
   MEMBERASSERT();
   GetClientRect( m_hWnd, &m_rClient );
   m_Surface.SetWinDC(GetDC(m_hWnd));
   m_Surface.SetFont(GetFont());

   // Start the heartbeat timer that drives the drawing.
   VERIFY(SetTimer(m_hWnd, c_uTimerID, c_uTimerPeriod, NULL));
   VERIFY(m_Thread.Create(this));
   return TRUE;
}

//===============================================================================================
// FUNCTION: OnSize
// PURPOSE:  Processes the WM_SIZE message.
//
void CTDrawWnd::OnSize()
{
   MEMBERASSERT();
   m_Thread.Suspend();

   // Resize the off-screen bitmap.
   GetClientRect( m_hWnd, &m_rClient );
   m_Surface.Resize(&m_rClient);

   // Start the background thread redrawing.
   Redraw();
}

//===============================================================================================
// FUNCTION: SetText
// PURPOSE:  Processes the WM_SETTEXT message.
//
void CTDrawWnd::SetText(LPCSTR pszText)
{
   MEMBERASSERT();
   ::DefWindowProc(m_hWnd, WM_SETTEXT, 0, LPARAM(pszText));
   Redraw();
}

//===============================================================================================
// FUNCTION: SetFont
// PURPOSE:  Processes the WM_SETFONT message.
//
void CTDrawWnd::SetFont(HFONT hFont, BOOL bRedraw)
{
   MEMBERASSERT();
   m_hFont = hFont;
   m_Surface.SetFont(GetFont());
   Redraw();
}

//===============================================================================================
// FUNCTION: SetBackground
// PURPOSE:  Sets the color that the background will get cleared to.
//
void CTDrawWnd::SetBackgroundColor(COLORREF rgb)
{
   MEMBERASSERT();
   m_Surface.SetBackgroundColor(rgb);
   Redraw();
}

//===============================================================================================
// FUNCTION: GetFont
// PURPOSE:  Processes the WM_GETFONT message.
//   
HFONT CTDrawWnd::GetFont()
{
   MEMBERASSERT();
   return m_hFont ? m_hFont : GetStockFont(ANSI_VAR_FONT);
}

//===============================================================================================
// FUNCTION: Draw
// PURPOSE:  Virtual function called by OnPaint to do the actual drawing.
//
void CTDrawWnd::Draw()
{
   MEMBERASSERT();
}

//===============================================================================================
// FUNCTION: OnPaint
// PURPOSE:  Processes the WM_PAINT message.
//
void CTDrawWnd::OnPaint()
{
   MEMBERASSERT();
   PAINTSTRUCT ps;
   BeginPaint(m_hWnd, &ps);

   m_Surface.SetDirty();
   m_Surface.Update(&ps.rcPaint);

   EndPaint(m_hWnd, &ps);
}

//===============================================================================================
// FUNCTION: Redraw
// PURPOSE:  Start the secondary thread redrawing.
//
void CTDrawWnd::Redraw()
{
   MEMBERASSERT();
   m_Thread.Suspend();
   m_Thread.Resume();
}

//===============================================================================================
// FUNCTION: Lock
// PURPOSE:  Lock the display surface and return the DC.
//
HDC CTDrawWnd::Lock()
{
   MEMBERASSERT();
   if (m_Thread.IsSuspended())
   {
      throw c_nINTERRUPT_EXCEPTION;
      return NULL;
   }
   return m_Surface.Lock();
}

//===============================================================================================
// FUNCTION: Unlock
// PURPOSE:  Unlock the display surface and mark it dirty for updating to the screen.
//
void CTDrawWnd::Unlock()
{
   MEMBERASSERT();
   m_Surface.SetDirty();
   m_Surface.Unlock();
}

//===============================================================================================
// FUNCTION: Relock
// PURPOSE:  Unlocks the surface for a short period and then relocks it.
//
HDC CTDrawWnd::Relock()
{
   MEMBERASSERT();

   Unlock();
   return Lock();
}

//===============================================================================================
// FUNCTION: OnTimer
// PURPOSE:  Called periodically by a windows timer.
//
void CTDrawWnd::OnTimer()
{
   MEMBERASSERT();

   // Update the screen if the surface is dirty.
   m_Surface.Update(&m_rClient);
}

//===============================================================================================
// FUNCTION: OnClose
// PURPOSE:  Processes the WM_CLOSE message.
//
void CTDrawWnd::OnClose()
{
   MEMBERASSERT();
   if (m_hWnd)
      DestroyWindow(m_hWnd);
}

//===============================================================================================
// FUNCTION: OnDestroy
// PURPOSE:  Processes the WM_DESTROY message.
//
void CTDrawWnd::OnDestroy()
{
   MEMBERASSERT();
   if (m_hOldPalette)
      ::SelectPalette(m_hdcWin, m_hOldPalette, FALSE);

   VERIFY(m_Thread.End());
   KillTimer(m_hWnd, c_uTimerID);
   SETOBJPTR(m_hWnd, NULL);
   SetHWND(NULL);
}

//===============================================================================================
// FUNCTION: Register
// PURPOSE:  Registers the CTDrawWndProc class.
// NOTES:    If a derived class overrides the Register() member function it must also have its
//           own class static flag to indicate whether registration has been completed ok, and
//           a unique class name provided by an overridden GetClassName() function.
//
BOOL CTDrawWnd::Register(HINSTANCE hInstance)
{
   static s_bRegistered = FALSE;
   if (s_bRegistered)
      return TRUE;
      
   // The dialog proc function.
   LRESULT CALLBACK TDrawWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

   WNDCLASS cls;
   memset(&cls, 0, sizeof(WNDCLASS));
   cls.hCursor       = LoadCursor(NULL, IDC_ARROW);
   cls.hIcon         = NULL;
   cls.lpszMenuName  = NULL;
   cls.hInstance     = hInstance;
   cls.lpszClassName = GetClassName();
   cls.hbrBackground = NULL;   // (HBRUSH)(COLOR_WINDOW+1);
   cls.lpfnWndProc   = TDrawWndProc;
   cls.style         = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_OWNDC;
   cls.cbWndExtra    = sizeof(void *);
   cls.cbClsExtra    = 0;

   s_bRegistered = RegisterClass(&cls);
   return s_bRegistered;
}

//===============================================================================================
// FUNCTION: CreateWnd
// PURPOSE:  Function for creating a Window associated with the CTDrawWnd object.
//
BOOL CTDrawWnd::CreateWnd(LPCSTR pszTitle, DWORD dwStyle, LPCRECT pRect, 
                          HWND hwndParent, int nWindowID, HINSTANCE hInstance,
                          DWORD dwExStyle)
{
   MEMBERASSERT();
   HWNDASSERT(hwndParent);
   ASSERT(m_hWnd==NULL);
      
   if (!Register(hInstance))
      return FALSE;

   if (dwExStyle==0)
      ::CreateWindow( GetClassName(), pszTitle, dwStyle, 
                      pRect->left, pRect->top, 
                      pRect->right-pRect->left, pRect->bottom-pRect->top,
                      hwndParent, (HMENU)nWindowID, hInstance, this );
   else                
      ::CreateWindowEx( dwExStyle, GetClassName(), pszTitle, dwStyle, 
                        pRect->left, pRect->top, 
                        pRect->right-pRect->left, pRect->bottom-pRect->top,
                        hwndParent, (HMENU)nWindowID, hInstance, this );

   // If the WM_NCCREATE processing has not set the window handling, we are in trouble.
   if (m_hWnd==NULL)
      return FALSE;

   // Get the DC and hold onto it. This assumes that the window was created with CS_OWNDC.
   m_hdcWin = GetDC(m_hWnd);
   return TRUE;
}

//===============================================================================================
// FUNCTION: DestroyWnd
// PURPOSE:  Destroys the window associated with the CTDrawWnd object and resets the object.
//
BOOL CTDrawWnd::DestroyWnd()
{
   MEMBERASSERT();
   if (!m_hWnd || !IsWindow(m_hWnd))
   {
      m_hWnd = NULL;
      return FALSE;
   }
      
   VERIFY(DestroyWindow(m_hWnd));
   return TRUE;
}

//===============================================================================================
// FUNCTION: ReplaceControl
// PURPOSE:  Function for replacing a place holder control in a dialog with a CTDrawWnd object.
//
BOOL CTDrawWnd::ReplaceControl(HWND hWnd, int nControlID)
{
   MEMBERASSERT();
   HWNDASSERT(hWnd);
   HWND hwndControl = GetDlgItem(hWnd, nControlID);
   if (!hwndControl)
      return NULL;

   // Grab the existing window text and font.      
   char szText[80];
   GetWindowText(hwndControl, szText, sizeof(szText));
   HFONT hFont = GetWindowFont(hwndControl);
   
   CRect rect;
   GetWindowRect(hwndControl, &rect);
   MapWindowRect(NULL, hWnd, &rect);
   DWORD dwStyle = WS_CHILD | WS_VISIBLE;
   int nWindowID = GetWindowID(hwndControl);
   HINSTANCE hInstance = GetWindowInstance(hWnd);

   if (!CreateWnd(szText, dwStyle, &rect, hWnd, nWindowID, hInstance))
      return FALSE;
   if (hFont)
      SetWindowFont(m_hWnd, hFont, TRUE);
   DestroyWindow(hwndControl);
   return TRUE;
}

//===============================================================================================
// FUNCTION: OnPaletteChanged
// PURPOSE:  Processes the WM_PALETTECHANGED message.
//
void CTDrawWnd::OnPaletteChanged(HWND hwndFocus) 
{
   MEMBERASSERT();
	if (hwndFocus != m_hWnd)
		SelectPalette(TRUE);
}

//===============================================================================================
// FUNCTION: OnQueryNewPalette
// PURPOSE:  Processes the WM_QUERYNEWPALETTE message.
//
BOOL CTDrawWnd::OnQueryNewPalette() 
{
   MEMBERASSERT();
	return SelectPalette(FALSE);
}

//===============================================================================================
// FUNCTION: SelectPalette
// PURPOSE:  Selects the surface's palette into the device as appropriate for foreground or background.
//
BOOL CTDrawWnd::SelectPalette(BOOL bBackground)
{
   MEMBERASSERT();
	HPALETTE hPalette = m_Surface.GetPalette();
   if (!hPalette)
		return FALSE;

   HPALETTE hOldPal = ::SelectPalette(m_hdcWin, hPalette, bBackground);
	if (::RealizePalette(m_hdcWin) > 0)
		InvalidateRect(m_hWnd, NULL, TRUE);

   if (m_hOldPalette==NULL)
      m_hOldPalette = hOldPal;

	return TRUE;
}

//===============================================================================================
// FUNCTION: DefWindowProc
// PURPOSE:  Calls the default procedure for processing messages.
//
LRESULT CTDrawWnd::DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   MEMBERASSERT();

   switch(uMsg)
   {
      case WM_NCCREATE:
         return OnNCCreate(LPCREATESTRUCT(lParam));

      case WM_CREATE:
         return OnCreate(LPCREATESTRUCT(lParam));

      case WM_TIMER:
         OnTimer();
         return 0;

      case WM_PAINT:
         OnPaint();
         return 0;

      case WM_ERASEBKGND:
         return 0;

      case WM_PALETTECHANGED:
         OnPaletteChanged(HWND(wParam));
         return 0;

      case WM_QUERYNEWPALETTE:
         return OnQueryNewPalette();

      case WM_CLOSE:
         OnClose();
         return 0;

      case WM_SETTEXT:
         SetText(LPCSTR(lParam));
         return TRUE;

      case WM_SETFONT:
         SetFont(HFONT(wParam), BOOL(lParam));
         return 0;

      case WM_GETFONT:
         return (LRESULT)GetFont();

      case WM_DESTROY:
         OnDestroy();
         return 0;

      case WM_DISPLAYCHANGE:
      case WM_SIZE:
         OnSize();
         return 0;

      default:
         return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
   }
}

//==============================================================================================
// FUNCTION: CTDrawWndProc
// PURPOSE:  Dialog proc for the window.
//
LRESULT CALLBACK TDrawWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   HWNDASSERT( hWnd );

   if (uMsg==WM_NCCREATE)
   {
      LPCREATESTRUCT lpCreateStruct = LPCREATESTRUCT(lParam);
      CTDrawWnd *pObj = (CTDrawWnd *)lpCreateStruct->lpCreateParams;
      if (!pObj)
         return FALSE;
      WPTRASSERT(pObj);
      SETOBJPTR(hWnd, pObj);
      pObj->SetHWND(hWnd);
   }

   CTDrawWnd *pObj = GETOBJPTR(hWnd);
   if (pObj)
      return pObj->DefWindowProc(uMsg, wParam, lParam);

   return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//###############################################################################################
//###############################################################################################
//###
//###    Thread class implimentation.
//###
//###############################################################################################
//###############################################################################################


//===============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Sets the m_hWnd variable and initializes the object.
//
CDrawThread::CDrawThread()
   : m_SuspendEvent(TRUE)
{
   MEMBERASSERT();
   m_pClient    = NULL;
   m_bNeeded    = TRUE;
   m_bSuspend   = TRUE;
   m_hThread    = NULL;
}

//===============================================================================================
// FUNCTION: Destructor
// PURPOSE:  Cleans up on destruction of the object.
//
CDrawThread::~CDrawThread()
{
   MEMBERASSERT();
   ASSERT(m_hThread==NULL);
}

//===============================================================================================
// FUNCTION: IsSuspended
// PURPOSE:  Returns the status of the suspend flag.
//
BOOL CDrawThread::IsSuspended() const
{
   MEMBERASSERT();
   return m_bSuspend;
}

//===============================================================================================
// FUNCTION: WaitUntilNeeded
// PURPOSE:  Waits until the thread is required.
//
BOOL CDrawThread::WaitUntilNeeded()
{
   MEMBERASSERT();

   // Check if the thread should be suspended.
   // TRACE("== Thread suspended ==.\n");
   m_bSuspend = TRUE;
   m_SuspendEvent.SetEvent();
   ::SuspendThread(m_hThread);
   // TRACE("== Thread resumed ==.\n");
   return m_bNeeded;
}

//===============================================================================================
// FUNCTION: Resume
// PURPOSE:  Resume the thread from its initial slumber, or its suspended state.
//
BOOL CDrawThread::Resume()
{
   MEMBERASSERT();
   // TRACE("ResumeThread()\n");
   ASSERT(m_bSuspend==TRUE);
   m_bSuspend = FALSE;
   m_SuspendEvent.ResetEvent();
   if (m_hThread)
      ::ResumeThread(m_hThread);

   // A little bit of sleep here allows the first portion of the draw code (up to the first Relock())
   // to get done before it is interrupted. This looks better when the window is being drag-sized.
   Sleep(0);
   return TRUE;
}

//===============================================================================================
// FUNCTION: Suspend
// PURPOSE:  Suspend the thread so that it does not chew up CPU cycles when not needed.
//
BOOL CDrawThread::Suspend()
{
   MEMBERASSERT();
   // TRACE("SuspendThread()\n");
   if (!m_bSuspend)
   {
      // Set the suspend flag to prevent stacked suspend calls.
      m_bSuspend = TRUE;

      // Wait for confirmation from the thread.
      m_SuspendEvent.WaitEvent();

      // As there is no object that is signaled when the thread actually goes into suspension,
      // we need to sleep a bit to make sure that the thread actually made it into suspension.
      Sleep(0);
   }
   return TRUE;
}

//===============================================================================================
// FUNCTION: End
// PURPOSE:  Terminate the thread.
//
BOOL CDrawThread::End()
{
   MEMBERASSERT();
   // TRACE("End()\n");
   // Set the suicide flag and resume the thread.
   Suspend();
   ASSERT(m_bNeeded==TRUE);
   m_bNeeded = FALSE;
   Resume();

   // A thread becomes signalled when it terminates.
   DWORD dwRval = WaitForSingleObject(m_hThread, 2000);
   ASSERT(dwRval==WAIT_OBJECT_0);

   CloseHandle(m_hThread);
   m_hThread = NULL;
   return TRUE;
}

//===============================================================================================
// FUNCTION: ThreadProc
// PURPOSE:  Thread procedure used to drive the data pump.
//
DWORD CDrawThread::ThreadProc()
{
   MEMBERASSERT();
   // TRACE("== Thread Started ==\n");

   // Wait until the thread is needed, suicide if not.
   while (WaitUntilNeeded())
   {
      try
      {
         m_pClient->Draw();
      }
      catch (...)
      {
         // TRACE("== Interrupted ==.\n");
      }
   }

   // TRACE("== Thread terminated ==\n");
   return 0;
}

//===============================================================================================
// FUNCTION: ThreadProc
// PURPOSE:  Thread proxy procedure used to call the object version.
//
static UINT WINAPI ThreadProxy( LPVOID pvTDrawWnd )
{
   // No MEMBERASSERT() -- static function.
   CDrawThread *pThread = (CDrawThread *)pvTDrawWnd;
   return pThread->ThreadProc();
}

//===============================================================================================
// FUNCTION: Create
// PURPOSE:  Create a thread in a suspended state..
//
BOOL CDrawThread::Create(CTDrawWnd *pClient)
{
   MEMBERASSERT();
   // TRACE("== Thread Created ==\n");

   // Save the client pointer and set the state flags.
   m_pClient  = pClient;
   m_bNeeded  = TRUE;
   m_bSuspend = TRUE;

   // Create the thread in a running state so that it runs straight into suspension.
   // Use _beginthreadex so that clients can call runtime functions during drawing.
   // The cast to a HANDLE is OK -- MFC does this in CWinThread.
   ASSERT(m_hThread==NULL);
   DWORD dwStack   = 2048;
   UINT  uThreadID = 0;
	m_hThread = (HANDLE)_beginthreadex(NULL, dwStack, ThreadProxy, this, 0, &uThreadID);  // CREATE_SUSPENDED

   if (!m_hThread)
   {
      SHOW_SYSTEM_ERROR(0);
      return FALSE;
   }

   // Wait for suspension.
   VERIFY(m_SuspendEvent.WaitEvent());

   // Wait a bit more just to be sure.
   Sleep(0);
   return TRUE;
}
