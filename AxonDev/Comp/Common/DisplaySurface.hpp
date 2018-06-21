//****************************************************************************************
//
//    Copyright (c) 1996 Axon Instruments.
//    All rights reserved.
//
//****************************************************************************************
// HEADER:  DISPSURF.HPP
// PURPOSE: Contains class definition for CDisplaySurface class and associated derived classes.
// AUTHOR:  BHI  Mar 1996
//

#ifndef INC_DISPSURF_HPP
#define INC_DISPSURF_HPP

#pragma once
#include "AfxGdiObj.hpp"
#include "AfxMT.hpp"

//===============================================================================================

class CDisplaySurface
{
private:    // Private member variables.
   mutable CCriticalSection m_CS;        // For syncronised access to the bitmap DC.

   BITMAPINFO      *m_pBitmapInfo;       // DIB header.
   BYTE            *m_pDibBits;	        // Pointer to the DIB bits.
   CSize            m_ActualSize;        // Actual physical size of the bitmap.
   HDC              m_hdcBM;             // Device context associated with this display surface
   HDC              m_hdcWin;            // Device context for the window.
   HBITMAP          m_hBitmap;           // Offscreen bitmap used to draw on.
   HBITMAP          m_hbmSave;           // Place to save Bitmap that was originally selected in the DC.
   CPalette         m_Palette;           // Palette used for display.
   int              m_nPaletteSize;      // Number of entries in the palette (0 if not 8bit mode).
   int              m_nScreenBPP;        // Number of bits per pixel in the screen DC.
   CRect            m_rDisplay;          // Current dimensions of the display surface in client coords.
   COLORREF         m_rgbBackground;     // Color to use for background.
   HFONT            m_hFont;             // Font to use on this surface.
   BOOL             m_bDirty;            // TRUE if the bitmap has been modified since last update.

private:    // Unimplimented copy constructor.
   const CDisplaySurface &operator=(const CDisplaySurface &C);

private:    // Private functions.
   void FreeBitmap();
   void CreateIdentityPalette(const LPPALETTEENTRY pRGBEntryIn, LPPALETTEENTRY pRGBIdentityOut, int nNumEntries);

public:     // Public interface.
   CDisplaySurface();
   CDisplaySurface(const CDisplaySurface &C);
   ~CDisplaySurface();

   BOOL IsOK() const;

   void SetWinDC(HDC hDC);
   HDC  GetWinDC() const;

   HDC  Lock() const;
   void Unlock() const;

   void SetBackgroundColor(COLORREF rgbBackground);
   void SetFont(HFONT hFont);

   void Resize( LPCRECT prDisplay );
   void Clear( LPCRECT pRect=NULL );
   void Update( LPCRECT prUpdate=NULL );

   HPALETTE GetPalette() const;

   void SetDirty();
   BOOL GetDirty() const;
};

//==============================================================================================
// FUNCTION: Lock.
// PURPOSE:  Returns the offscreen device context.
//
inline HDC CDisplaySurface::Lock() const
{
   MEMBERASSERT();
   m_CS.Lock();
   return m_hdcBM;
}

//===============================================================================================
// FUNCTION: Unlock
// PURPOSE:  .
//
inline void CDisplaySurface::Unlock() const
{
   MEMBERASSERT();
   m_CS.Unlock();
}

#endif

