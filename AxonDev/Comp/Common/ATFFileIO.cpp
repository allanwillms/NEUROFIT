//***********************************************************************************************
//
//    Copyright (c) 1997 Axon Instruments.
//    All rights reserved.
//
//***********************************************************************************************
// MODULE:  ATFFileIO.cpp
// PURPOSE: Contains class implementation for CATFFileIO class. 
//          This is a class wrapper for the ATF file writing API.
// AUTHOR:  BHI  May 1997
//
#include "wincpp.hpp"
#include "ATFFileIO.hpp"
#include "\AxonDev\Comp\axatffio32\axatffio32.h"

extern HINSTANCE g_hInstance;

/////////////////////////////////////////////////////////////////////////
// ATF writing functions


//===============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Object initialization.
//
CATFFileIO::CATFFileIO()
{
   MEMBERASSERT();
   m_hFile      = ATF_INVALID_HANDLE;
   m_nLastError = 0;
   m_szFilename[0] = '\0';
}

//===============================================================================================
// FUNCTION: Destructor
// PURPOSE:  Object cleanup.
//
CATFFileIO::~CATFFileIO()
{
   MEMBERASSERT();
   Close();
}                         

//===============================================================================================
// FUNCTION: WriteOpen
// PURPOSE:  Opens an ATF file for writing.
//
BOOL CATFFileIO::WriteOpen(LPCSTR pszFilename, int nColumns)
{
   MEMBERASSERT();
   ASSERT( m_hFile == ATF_INVALID_HANDLE );   

   int nError = 0;
   if (!ATF_OpenFile(pszFilename, ATF_WRITEONLY|ATF_OVERWRTIFEXIST, &nColumns, &m_hFile, &nError))
      return SetLastError(nError);

   AXU_strncpyz( m_szFilename, pszFilename, sizeof( m_szFilename ));
   return TRUE;
}

//===============================================================================================
// FUNCTION: Close
// PURPOSE:  Closes an open ATF file.
//
void CATFFileIO::Close()
{
   MEMBERASSERT();
   if (m_hFile != ATF_INVALID_HANDLE)
   {
      ATF_CloseFile(m_hFile);
      m_hFile = ATF_INVALID_HANDLE;
   }
}

//===============================================================================================
// FUNCTION: WriteHeader
// PURPOSE:  Writes a header record out to the file.
//
BOOL CATFFileIO::WriteHeader(LPCSTR pszText)
{
   MEMBERASSERT();

   int nError = 0;
   if (!ATF_WriteHeaderRecord(m_hFile, pszText, &nError))
      return SetLastError(nError);

   ATF_WriteEndOfLine(m_hFile, &nError);
   return TRUE;
}

//===============================================================================================
// FUNCTION: WriteHeader
// PURPOSE:  Writes a header record out to the file.
//
BOOL CATFFileIO::WriteHeader(UINT uTplt, ...)
{
   MEMBERASSERT();
   char sz[256];

   // Start the va_list
   va_list args;
   va_start( args, uTplt );

   // Send the call to the VFormatString implementation
   AXU_VFormatStringEx( g_hInstance, sz, MAKEINTRESOURCE(uTplt), args );

   // End the va_list
   va_end( args );

   return WriteHeader(sz);
}

//===============================================================================================
// FUNCTION: SetColumnHeading
// PURPOSE:  Set the next column heading. Column headings are set concecutively.
//
BOOL CATFFileIO::SetColumnHeading(LPCSTR pszTitle, LPCSTR pszUnits)
{
   MEMBERASSERT();
   int nError = 0;
   if (!ATF_SetColumnTitle(m_hFile, pszTitle, &nError))
      return SetLastError(nError);

   if (!ATF_SetColumnUnits(m_hFile, pszUnits ? pszUnits : "", &nError))
      return SetLastError(nError);

   return TRUE;
}

//===============================================================================================
// FUNCTION: WriteData
// PURPOSE:  Writes an array of floats out to the file.
//
BOOL CATFFileIO::WriteData(float *pfArray, UINT uCount, LPCSTR pszComment)
{
   MEMBERASSERT();
   int nError = 0;
   if (!ATF_WriteDataRecordArrayFloat(m_hFile, uCount, pfArray, &nError))
      return SetLastError(nError);

   if (pszComment &&
       !ATF_WriteDataComment(m_hFile, pszComment, &nError))
      return SetLastError(nError);

   if (!ATF_WriteEndOfLine(m_hFile, &nError))
      return SetLastError(nError);

   return TRUE;
}

//===============================================================================================
// FUNCTION: WriteData
// PURPOSE:  Writes an array of floats out to the file.
//
BOOL CATFFileIO::WriteData(double *pdArray, UINT uCount, LPCSTR pszComment)
{
   MEMBERASSERT();

   int nError = 0;
   if (!ATF_WriteDataRecordArray(m_hFile, uCount, pdArray, &nError))
      return SetLastError(nError);

   if (pszComment &&
       !ATF_WriteDataComment(m_hFile, pszComment, &nError))
      return SetLastError(nError);

   if (!ATF_WriteEndOfLine(m_hFile, &nError))
      return SetLastError(nError);

   return TRUE;
}

//===============================================================================================
// FUNCTION: WriteData
// PURPOSE:  Writes an array of floats out to the file.
//
BOOL CATFFileIO::WriteData( LPCSTR pszData, LPCSTR pszComment)
{
   MEMBERASSERT();

   int nError = 0;
   if (!ATF_WriteDataRecord( m_hFile, pszData, &nError))
      return SetLastError(nError);

   if (pszComment &&
       !ATF_WriteDataComment(m_hFile, pszComment, &nError))
      return SetLastError(nError);

   if (!ATF_WriteEndOfLine(m_hFile, &nError))
      return SetLastError(nError);

   return TRUE;
}

//===============================================================================================
// FUNCTION: WriteDataNoEOL
// PURPOSE:  Write a data item leaving the end of line open.
//
BOOL CATFFileIO::WriteDataNoEOL(LPCSTR pszData)
{
   MEMBERASSERT();
   
   int nError = 0;
   if (!ATF_WriteDataRecord(m_hFile, pszData, &nError))
      return SetLastError(nError);

   return TRUE;
}

//===============================================================================================
// FUNCTION: WriteDataNoEOL
// PURPOSE:  Write a data item leaving the end of line open.
//
BOOL CATFFileIO::WriteDataNoEOL(double dData)
{
   MEMBERASSERT();

   int nError = 0;
   if (!ATF_WriteDataRecord1(m_hFile, dData, &nError))
      return SetLastError(nError);

   return TRUE;
}

//===============================================================================================
// FUNCTION: WriteEndOfLine
// PURPOSE:  Writes an end of line to the header or data line.
//
BOOL CATFFileIO::WriteEndOfLine()
{
   MEMBERASSERT();

   int nError = 0;
   if (!ATF_WriteEndOfLine(m_hFile, &nError))
      return SetLastError(nError);

   return TRUE;
}

//===============================================================================================
// FUNCTION: SetLastError
// PURPOSE:  Convenience function to set the error value and return FALSE.
//
BOOL CATFFileIO::SetLastError(int nError)
{
   MEMBERASSERT();
   m_nLastError = nError;
   return FALSE;
}

//===============================================================================================
// FUNCTION: GetLastError
// PURPOSE:  Accessor function for the last error code.
//
int CATFFileIO::GetLastError() const
{
   MEMBERASSERT();
   return m_nLastError;
}

/////////////////////////////////////////////////////////////////////////
// ATF reading functions

//==============================================================================================
// FUNCTION: ReadOpen
// PURPOSE:  Open a file for reading
//
BOOL CATFFileIO::ReadOpen( LPCSTR pszFileName, int *pnColumns, int *pnHeaders )
{
   MEMBERASSERT();    
   ASSERT( m_hFile == ATF_INVALID_HANDLE );

   int nError = 0, nColumns = 0;
   if (!ATF_OpenFile( pszFileName, ATF_READONLY, &nColumns, &m_hFile, &nError))
      return SetLastError(nError);

   if (pnColumns)
      *pnColumns = nColumns;

   if (pnHeaders && !ATF_GetNumHeaders(m_hFile, pnHeaders, &nError))
      return SetLastError(nError);

   return TRUE;   
}

//==============================================================================================
// FUNCTION: ReadHeader
// PURPOSE:  Read a header
//
BOOL CATFFileIO::ReadHeader( LPSTR pHeaderText, int nMaxSize)
{
   MEMBERASSERT();

   int nError = 0;
   if (!ATF_ReadHeaderNoQuotes( m_hFile, pHeaderText, nMaxSize, &nError))                             
      return SetLastError(nError);
                           
   return TRUE;
}

//==============================================================================================
// FUNCTION: GetColumnHeading
// PURPOSE:  Get the heading string(s) for a particular column.
//
BOOL CATFFileIO::GetColumnHeading(int nColumn, LPSTR pszTitleText, int nMaxTitle, 
                                  LPSTR pszUnitsText, int nMaxUnit)
{
   MEMBERASSERT();

   int nError = 0;
   if (pszTitleText && nMaxTitle)
      if ( !ATF_GetColumnTitle( m_hFile, nColumn, pszTitleText, nMaxTitle, &nError ))
         return SetLastError(nError);

   if (pszUnitsText && nMaxUnit)
      if ( !ATF_GetColumnUnits( m_hFile, nColumn, pszUnitsText, nMaxUnit, &nError ))
         return SetLastError( nError );

   return TRUE;
}

//==============================================================================================
// FUNCTION: CountDataLines
// PURPOSE:  Counts the number of lines of data in the data section
//
BOOL CATFFileIO::CountDataLines(long *plNumLines)
{
   MEMBERASSERT();

   int nError = 0;
   if( !ATF_CountDataLines( m_hFile, plNumLines, &nError))
      return SetLastError( nError );

   return TRUE;
}

//==============================================================================================
// FUNCTION: ReadData
// PURPOSE:  Read an array of data from an ATF file.
//
BOOL CATFFileIO::ReadData( int nCount, double *pdVals, char *pszComment, int nMaxLen )
{
   MEMBERASSERT();

   int nError = 0;
   if(!ATF_ReadDataRecordArray( m_hFile, nCount, pdVals, pszComment, nMaxLen, &nError))                                                                   
      return SetLastError( nError );

   return TRUE;
}   

//==============================================================================================
// FUNCTION: ReadData
// PURPOSE:  Read data from the ATF file
//
BOOL CATFFileIO::ReadData( LPSTR pszText, int nMaxLen )
{
   MEMBERASSERT();

   int nError = 0;
   if (!ATF_ReadDataRecord( m_hFile, pszText, nMaxLen, &nError))
      return SetLastError( nError );

   return TRUE;
}   

//===============================================================================================
// FUNCTION: Rewind
// PURPOSE:  Rewind the file so that the data portion can be re-read.
//
BOOL CATFFileIO::Rewind()
{
   MEMBERASSERT();
   
   int nError = 0;
   if (!ATF_RewindFile( m_hFile, &nError))
      return SetLastError( nError );

   return TRUE;
}

//==============================================================================================
// FUNCTION: BuildErrorText
// PURPOSE:  Build error text string to discribe the last error
//
void CATFFileIO::BuildErrorText( LPSTR szTxtBuf, int nMaxLen ) const
{
   MEMBERASSERT();

   ATF_BuildErrorText( m_nLastError, m_szFilename, szTxtBuf, nMaxLen );
}

//==============================================================================================
// FUNCTION: GetFileName
// PURPOSE:  Returns the file name
//
LPCSTR CATFFileIO::GetFileName() const
{
   MEMBERASSERT();
   return m_szFilename;
}
