//***********************************************************************************************
//
//    Copyright (c) 1998 Axon Instruments.
//    All rights reserved.
//
//***********************************************************************************************
// MODULE:  Tooltip.HPP
// PURPOSE: 
// AUTHOR:  BHII  Jul 1998
//

#ifndef INC_TOOLTIP_HPP
#define INC_TOOLTIP_HPP

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <commctrl.h>

#ifdef _UNICODE
struct CToolInfo : public tagTOOLINFOW
#else
struct CToolInfo : public tagTOOLINFOA
#endif
{
public:
   CToolInfo() 
   {
      memset(this, 0, sizeof(*this));
      cbSize   = sizeof(TOOLINFO);
      lpszText = szText;
   }
   TCHAR szText[256];
};

class CTooltip
{
private:   // Attributes
   HWND  m_hWnd;
   HWND  m_hwndNotify;
   UINT  m_uAddToolFlags;

private:   // Unimplemented copy functions.
   CTooltip(const CTooltip &);
   const CTooltip &operator=(const CTooltip &);

private:   // Private functions
   void FillInToolInfo(CToolInfo &ti, HWND hWnd) const;
   void FillInToolInfo(CToolInfo &ti, HWND hwndParent, UINT uIDTool, LPCRECT pRect=NULL) const;

public:    // Public interface

// Constructors
   CTooltip();
   ~CTooltip();

   BOOL Create(HWND hParentWnd, DWORD dwStyle = 0);
   void Destroy();

   void SetNotifyWnd(HWND hwndNotify);
   HWND GetNotifyWnd() const;

   void SetAddToolFlags(UINT uFlags);
   UINT GetAddToolFlags() const;

// Attributes
   void GetText(HWND hWnd, LPSTR pszText, UINT uTextLen) const;
   void GetText(HWND hwndParent, UINT nIDTool, LPSTR pszText, UINT uTextLen) const;

   BOOL GetToolInfo(HWND hWnd, CToolInfo& ToolInfo) const;
   BOOL GetToolInfo(HWND hwndParent, UINT nIDTool, CToolInfo& ToolInfo) const;
   BOOL GetToolInfoByIndex(UINT uToolInx, CToolInfo& ToolInfo) const;

   void SetToolRect(HWND hwndParent, UINT nIDTool, LPCRECT pRect);

   void SetToolInfo(LPTOOLINFO lpToolInfo);
   int  GetToolCount() const;

// Operations
   void Activate(BOOL bActivate);
   void Clear();

   BOOL AddTool(TOOLINFO *pti);
   BOOL AddTool(HWND hWnd, LPCTSTR lpszText = LPSTR_TEXTCALLBACK);
   BOOL AddTool(HWND hwndParent, UINT nIDTool, LPCRECT pRect, LPCTSTR lpszText = LPSTR_TEXTCALLBACK);

   void DelTool(TOOLINFO *pti);
   void DelTool(HWND hWnd);
   void DelTool(HWND hwndParent, UINT nIDTool);

   BOOL HitTest(HWND hWnd, POINT pt, LPTOOLINFO lpToolInfo) const;
   void RelayEvent(LPMSG lpMsg);

   void SetDelayTime(UINT uDuration, UINT nDelay);

   void UpdateTipText(HWND hWnd, LPCTSTR lpszText);
   void UpdateTipText(HWND hwndParent, UINT nIDTool, LPCTSTR lpszText);

   int  SetMaxTipWidth(int nWidth);
   int  GetMaxTipWidth();

   BOOL HookMessages();
   void UnhookMessages();
};

#endif      // INC_TOOLTIP_HPP

