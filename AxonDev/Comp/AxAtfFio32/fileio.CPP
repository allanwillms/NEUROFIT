//***********************************************************************************************
//                                                                            
//   Written 1990 - 1996 by AXON Instruments Inc.                             
//                                                                            
//   This file is not protected by copyright. You are free to use, modify     
//   and copy the code in this file.                                          
//                                                                            
//***********************************************************************************************
//
// MODULE:  FILEIO.CPP
// PURPOSE: Low level routines for buffered file I/O.
// 
// An ANSI C compiler should be used for compilation.
// (e.g. CL -c AXATFFIO32.CPP)
//
#include "wincpp.hpp"
#include "atfintl.h"
#include "axatffio32.h"

//===============================================================================================
// FUNCTION: _GetRootDir
// PURPOSE:  Extracts the root directory of a full or partial path.
// FIXFIX:   Fix this to cope with UNC names when GetDiskFreeSpace does.
//
static BOOL _GetRootDir(LPCSTR pszFileName, LPSTR pszRoot, UINT uRootLen)
{
   // Build the path to the drive's root directory.
   char szRootDir[_MAX_DRIVE+2];
   char szFullPath[_MAX_PATH], *pszTitle;
   GetFullPathName(pszFileName, sizeof(szFullPath), szFullPath, &pszTitle);
   if (!isalpha(szFullPath[0]))
      return FALSE;

   sprintf(szRootDir, "%c:\\", szFullPath[0]);   
   strncpy(pszRoot, szRootDir, uRootLen-1);
   pszRoot[uRootLen-1] = '\0';
   return TRUE;
}  


//===============================================================================================
// FUNCTION: _AllocReadWriteBuffer
// PURPOSE:  Allocate read/write buffers for this file
//
static BOOL AllocReadWriteBuffer(ATF_FILEINFO *pATF, DWORD dwDesiredAccess)
{
   WPTRASSERT(pATF);

   // init all settings:
   pATF->lBufSize       = 0L;
   pATF->lPos           = 0L;
   pATF->lBufReadLimit  = 0L;
   pATF->pszBuf         = NULL;
   pATF->bRead          = TRUE;

   // if querying only:
   if (dwDesiredAccess == 0)
      return TRUE;

   char szRootDir[_MAX_DRIVE+2];
   if (_GetRootDir(pATF->pszFileName, szRootDir, sizeof(szRootDir)))
   {
      DWORD dwSectorsPerCluster     = 0;
      DWORD dwBytesPerSector        = 0;
      DWORD dwNumberOfFreeClusters  = 0;
      DWORD dwTotalNumberOfClusters = 0;
      GetDiskFreeSpace(szRootDir, &dwSectorsPerCluster, &dwBytesPerSector, 
                       &dwNumberOfFreeClusters, &dwTotalNumberOfClusters);
      pATF->lBufSize = min((dwSectorsPerCluster * dwBytesPerSector), (long)ATF_MAX_BUFFER_SIZE);
      ASSERT(pATF->lBufSize > 0);
   }
   else
      pATF->lBufSize = ATF_MAX_BUFFER_SIZE;

   // Allocate one more than the size for zero termination.
   pATF->pszBuf = (char *)calloc(pATF->lBufSize+1, sizeof(char));
   if (pATF->pszBuf == NULL)
   {
      pATF->lBufSize   = 0L;
      return FALSE;
   }

   pATF->lPos          = pATF->lBufSize;    // empty read buffer
   pATF->lBufReadLimit = pATF->lBufSize;
   return TRUE;
}

//===============================================================================================
// FUNCTION: _FreeReadWriteBuffer
// PURPOSE:  Free the read/write buffers used by this file; flushes write buffer to disk if necessary
//
static BOOL FreeReadWriteBuffer(ATF_FILEINFO *pATF)
{
   WPTRASSERT(pATF);

   DWORD dwBytesWritten = 0;
   if (!pATF->bRead && pATF->lPos != 0L)
      WriteFile(pATF->hFile, pATF->pszBuf, pATF->lPos, &dwBytesWritten, NULL);

   if (pATF->pszBuf)
      free(pATF->pszBuf);
   pATF->pszBuf        = NULL;
   pATF->lBufSize      = 0L;
   pATF->lPos          = 0L;
   pATF->lBufReadLimit = 0L;
   pATF->bRead         = TRUE;
   return TRUE;
}


//===============================================================================================
// FUNCTION:         _CreateFileBuf
// PURPOSE:          Buffered version of CreateFile - creates appropriate buffers
// PARAMETERS:
//    pATF           Pointer to ATF_FILEINFO structure containing ATF state information (just filename at this point!)
//    ...            CreateFile parms which we ignore (missing lpFileName - derived from pATF), except
//                   for dwDesiredAccess.  If GENERIC_READ or GENERIC_WRITE, we allocate a buffer
// RETURNS:          Handle of file; INVALID_HANDLE_VALUE if failure;        

HANDLE CreateFileBuf(ATF_FILEINFO *pATF, DWORD dwDesiredAccess, DWORD dwShareMode,
                     LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, 
                     DWORD dwFlagsAndAttributes, HANDLE hTemplateFile )
{
   pATF->hFile = CreateFile(pATF->pszFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
                            dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
   if (pATF->hFile != INVALID_HANDLE_VALUE)
   {
      // allocate buffer, initialize flags:
      if (!AllocReadWriteBuffer(pATF, dwDesiredAccess))
      {
         CloseHandle(pATF->hFile);         
         pATF->hFile = INVALID_HANDLE_VALUE;
      }
   }
   return pATF->hFile;
}


//===============================================================================================
// FUNCTION:         _CloseHandleBuf
// PURPOSE:          Buffered version of CloseHandle - flushes and cleans up buffers
// PARAMETERS:
//    pATF           Pointer to ATF_FILEINFO structure containing ATF state information (just filename at this point!)
// RETURNS:          TRUE on success; FALSE on failure

BOOL CloseHandleBuf(ATF_FILEINFO *pATF)
{
   BOOL bReturn = FreeReadWriteBuffer(pATF);
   return CloseHandle(pATF->hFile) && bReturn;
}


//===============================================================================================
// FUNCTION:         WriteFileBuf
// PURPOSE:          Buffered version of WriteFile
// PARAMETERS:
//    pATF           Pointer to ATF_FILEINFO structure containing ATF state information
//    pvBuffer       Buffer containing data to be written
//    dwBytes        Number of bytes to write
//    pdwWritten     Pointer to DWORD used to catch the number of bytes written to the file
//    lpOverlapped   Pointer to overlapped structure used to communicate requests of overlapped i/o (may be NULL)
// RETURNS:          
//    TRUE           on success
//    FALSE          on failure

BOOL WriteFileBuf(ATF_FILEINFO *pATF, LPCVOID pvBuffer, DWORD dwBytes, DWORD *pdwWritten, LPOVERLAPPED lpOverlapped)
{
   WPTRASSERT(pATF);

   long  lBufSize    = pATF->lBufSize;
   char *pszWriteBuf = pATF->pszBuf;

   // perform write if buffer size is 0:
   if (lBufSize == 0L)
      return WriteFile(pATF->hFile, pvBuffer, dwBytes, pdwWritten, lpOverlapped);

   // switch to write mode:
   if (pATF->bRead)
   {
      pATF->bRead = FALSE;
      pATF->lPos  = 0;
   }

   // determine free size left in buffer:
   long lFreeSize = lBufSize - pATF->lPos;
   ASSERT(lFreeSize > 0L);

   // move up to a single buffer
   long lMoveSize = min((DWORD)lFreeSize, dwBytes);
   memcpy(pszWriteBuf + pATF->lPos, pvBuffer, lMoveSize);
   pATF->lPos += lMoveSize;

   // case 1:  doesn't fill buffer
   if (pATF->lPos < lBufSize)
   {
      if (pdwWritten)
         *pdwWritten = dwBytes;
      return TRUE;
   }

   // write initial buffer - results handled in case 2 and 3:
   DWORD dwBytesWritten = 0;
   BOOL  bReturn = WriteFile(pATF->hFile, pszWriteBuf, lBufSize, &dwBytesWritten, lpOverlapped);
   
   // case 2:  fills buffer, less than one buffer overflow (write one, move the rest)
   if (dwBytes - (DWORD)lMoveSize < (DWORD)lBufSize)
   {
      if (dwBytes - lMoveSize > 0L)
         memcpy(pszWriteBuf, ((BYTE *)pvBuffer + lMoveSize), dwBytes-lMoveSize);
      
      pATF->lPos = dwBytes - lMoveSize;
      if (pdwWritten)
         *pdwWritten = dwBytes;
      return bReturn;
   }

   // case 3:  multiple buffer's worth (write mem buffer, write the remainder, reset internals)
   if (bReturn)
   {
      bReturn = WriteFile(pATF->hFile, ((BYTE *)pvBuffer + lMoveSize), 
                          dwBytes - lMoveSize, &dwBytesWritten, lpOverlapped);
      if (pdwWritten)
         *pdwWritten = dwBytes;
   }
   else if (pdwWritten)
      *pdwWritten = dwBytesWritten;
      
   pATF->lPos = 0L;
   return bReturn;
}


//===============================================================================================
// FUNCTION:         ReadFileBuf
// PURPOSE:          Buffered version of ReadFile
// PARAMETERS:
//    pATF           Pointer to ATF_FILEINFO structure containing ATF state information
//    pvBuffer       Buffer to contain dwBytes read from file
//    dwBytes        Number of bytes to read
//    pdwRead        Pointer to DWORD used to catch the number of bytes written to the file
//    lpOverlapped   Pointer to overlapped structure used to communicate requests of overlapped i/o (may be NULL)
// RETURNS:          
//    TRUE           on success (*pdwRead == 0 -> EOF)
//    FALSE          on failure (error condition)

BOOL ReadFileBuf(ATF_FILEINFO *pATF, LPVOID pvBuffer, DWORD dwBytes, DWORD *pdwRead, LPOVERLAPPED lpOverlapped)
{
   WPTRASSERT(pATF);

   // perform read if buffer size is 0:
   if (pATF->lBufSize == 0L)
      return ReadFile(pATF->hFile, pvBuffer, dwBytes, pdwRead, lpOverlapped);

   // switch to read mode:
   if (!pATF->bRead)
   {
      DWORD dwBytesWritten;

      // commit current cache:
      if (pATF->lPos > 0L)
         if (!WriteFile(pATF->hFile, pATF->pszBuf, pATF->lPos, &dwBytesWritten, NULL))
            return FALSE;

      pATF->bRead         = TRUE;
      pATF->lPos          = pATF->lBufSize;
      pATF->lBufReadLimit = pATF->lBufSize;
   }

   DWORD dwBytesRead;
   BOOL  bReturn;
   long  lBufSize   = pATF->lBufSize;
   char *pszReadBuf = pATF->pszBuf;

   // determine amount left in buffer:
   long lBytesInBuf = pATF->lBufReadLimit - pATF->lPos;
   ASSERT(lBytesInBuf >= 0L);

   // move up to a single buffer
   long lMoveSize = min((DWORD)lBytesInBuf, dwBytes);
   if (lMoveSize > 0L)
   {
      memcpy(pvBuffer, pszReadBuf + pATF->lPos, lMoveSize);
      pATF->lPos += lMoveSize;
   }

   // case 1:  request doesn't run past the end of the buffer
   if (pATF->lPos < pATF->lBufReadLimit)
   {
      if (pdwRead)
         *pdwRead = dwBytes;
      return TRUE;
   }

   // case 2: request runs past end of buffer, and wants more than (or =) another buffer's worth:
   //         (perform a full read; leaves buffer empty)
   if (dwBytes - (DWORD)lMoveSize >= (DWORD)pATF->lBufReadLimit)
   {
      bReturn = ReadFile(pATF->hFile, ((BYTE *)pvBuffer + lMoveSize), 
                         dwBytes - lMoveSize, &dwBytesRead, lpOverlapped);
      if (pdwRead)
         *pdwRead = lMoveSize + dwBytesRead;
      pATF->lPos           = lBufSize;
      pATF->lBufReadLimit  = lBufSize;
      return bReturn;
   }

   // case 3: request runs past end of buffer, and wants less than another buffer's worth:
   //        (read in another buffer, copy wanted portion, advance lPos)

   bReturn = ReadFile(pATF->hFile, pszReadBuf, lBufSize, &dwBytesRead, lpOverlapped);
   if (bReturn)
   {
      pATF->lBufReadLimit = dwBytesRead;
      
      int nMoveAmount = min((int)(dwBytes - lMoveSize), pATF->lBufReadLimit);
      memcpy((BYTE *)pvBuffer + lMoveSize, pszReadBuf, nMoveAmount); 
      if (pdwRead)
         *pdwRead = lMoveSize + nMoveAmount;
      pATF->lPos = nMoveAmount;
   }
   else 
   {
      if (pdwRead)
         *pdwRead = lMoveSize;
      pATF->lPos = lBufSize;
   }
   return bReturn;
}

               
//===============================================================================================
// FUNCTION:         _SetFilePointerBuf
// PURPOSE:          Buffered version of SetFilePointer
// PARAMETERS:
//    pATF           Pointer to ATF_FILEINFO structure containing ATF state information
//    lToMove        Amount to move from position specified in dwMoveMethod; negative means move backwards
//    plDistHigh     High order word of 64-bit distance to move.  Should be NULL
//    dwMoveMethod   Method to move:  FILE_BEGIN   - move lToMove bytes from beginning of file
//                                    FILE_CURRENT - move lToMove bytes from current position
//                                    FILE_END     - move lToMove bytes from end of file
// RETURNS:          Offset of new position from beginning of file (0xFFFFFFFF if failure)

DWORD SetFilePointerBuf(ATF_FILEINFO *pATF, long lToMove, PLONG plDistHigh, DWORD dwMoveMethod)
{
   WPTRASSERT(pATF);

   DWORD dwBytesWritten;

   // move real file position to lPos:
   if (pATF->bRead) 
   {
      if (SetFilePointer(pATF->hFile, pATF->lPos - pATF->lBufReadLimit, NULL, FILE_CURRENT) == 0xFFFFFFFF)
         return 0xFFFFFFFF;
   }
   // flush write buffer if non-empty - this positions the file pointer appropriately.
   else
   {
      if (pATF->lPos != 0L)
      {
         if (!WriteFile(pATF->hFile, pATF->pszBuf, pATF->lPos, &dwBytesWritten, NULL))
            return 0xFFFFFFFF;
      }
   }

   pATF->bRead          = TRUE;
   pATF->lPos           = pATF->lBufSize;
   pATF->lBufReadLimit  = pATF->lBufSize;
   return SetFilePointer(pATF->hFile, lToMove, plDistHigh, dwMoveMethod);
}


//===============================================================================================
// FUNCTION: SetLineTerminator
// PURPOSE:  Sets the line terminator to use on this file.
// NOTES:    This call assumes that if the file only contains \r characters as
//           line terminators, that there will be at least one '\r' in the first read of the file.
//           This should be reasonably safe as the first line of an ATF file should be "ATF\t1.0 \r".
//
inline char GetLineTerminator(LPSTR psz)
{
   return strchr(psz, '\n') ? '\n' : '\r';
}

//===============================================================================================
// FUNCTION: getsUnBuf
// PURPOSE:  Unbuffered version of gets
// RETURNS:  ZERO          on success
//           GETS_EOF   on EOF
//           GETS_ERROR on error
//
static int getsUnBuf(ATF_FILEINFO *pATF, LPSTR pszString, DWORD dwBufSize)
{
   WPTRASSERT(pATF);
   ASSERT(dwBufSize > 1L);  // Must be at least one character and a '\0';

   DWORD dwToRead = dwBufSize;

   // Zero terminate the buffer at the last element and reduce the length count
   // to be sure that we are returning a zero term8inated string.
   dwToRead--;
   pszString[dwToRead] = '\0';
   LPSTR pszThisRead = pszString;

   while (dwToRead > 0L)
   {
      // Do the read.
      DWORD dwBytesToRead = min(MAX_READ_SIZE, dwToRead);
      DWORD dwBytesRead = 0L;
      if (!ReadFileBuf(pATF, pszThisRead, dwBytesToRead, &dwBytesRead, NULL))
         return GETS_ERROR;
      if (dwBytesRead == 0L)
         return GETS_EOF;

      // Zero terminate the read block after the last byte read.
      // No bounds problem because we predecremented the string size
      // up front to allow for a trailing '\0'.
      pszThisRead[dwBytesRead] = '\0';

      // If the line terminator has not been set, set it now.
      if (pATF->cLineTerm == '\0')
         pATF->cLineTerm = GetLineTerminator(pszString);

      // look for a line terminator.
      LPSTR pszTerm = strchr(pszThisRead, pATF->cLineTerm);
      if (pszTerm)
      {
         // Zero out the terminator and step on past it.
         *pszTerm++ = '\0';

         // Set the count of bytes to back up in the file.
         int nCount = (pszThisRead + dwBytesRead) - pszTerm;

         // adjust file position if we find a line terminator before the end of the buffer we have just read;
         if (nCount < 0)
            SetFilePointerBuf(pATF, nCount, NULL, FILE_CURRENT);   
   
         break;
      }
      dwToRead -= dwBytesRead;
      pszThisRead += dwBytesRead;
   }

   // Take out the last character if it is '\r'.
   // (present if \r\n pairs terminate lines)
   int l = strlen(pszThisRead);
   if (l && (pszThisRead[l-1]=='\r'))
   {
      --l;
      pszThisRead[l] = '\0';
   }
   
   return (DWORD(l) < dwBufSize-1) ? 0 : GETS_NOEOL;
}


//===============================================================================================
// FUNCTION: getsBuf
// PURPOSE:  Buffered version of gets -- line terminated are removed from the returned string.
// RETURNS:  ZERO          on success
//           GETS_EOF   on EOF
//           GETS_ERROR on error

int getsBuf(ATF_FILEINFO *pATF, LPSTR pszString, DWORD dwBufSize)
{
   WPTRASSERT(pATF);

   // *******************************************************************************
   // check for unbuffered status:
   if (pATF->lBufSize == 0)
      return getsUnBuf(pATF, pszString, dwBufSize);

   DWORD dwToRead = dwBufSize;

   // *******************************************************************************
   // switch to read mode, if necessary:
   if (!pATF->bRead)
   {
      DWORD    dwBytesWritten;

      // commit current cache:
      if (pATF->lPos > 0)
         if (!WriteFile(pATF->hFile, pATF->pszBuf, pATF->lPos, &dwBytesWritten, NULL))
            return GETS_ERROR;

      pATF->bRead = TRUE;
      pATF->lPos  = pATF->lBufSize;
      pATF->lBufReadLimit = pATF->lBufSize;
   }

   // *******************************************************************************
   // process:

   dwToRead--;       // for terminating 0
   pszString[dwToRead] = '\0';

   LPSTR pszReturnBuf = pszString;
   LPSTR pszReadBuf   = pATF->pszBuf;

   while (dwToRead > 0L)
   {
      // determine amount left in buffer:
      long lBytesInBuf = pATF->lBufReadLimit - pATF->lPos;
      ASSERT(lBytesInBuf >= 0L);

      // move up to a single buffer
      long lMoveSize = min(lBytesInBuf, (long)dwToRead);

      if (lMoveSize > 0)
      {
         // look for a line terminator
         LPSTR pszStart = pszReadBuf + pATF->lPos;
         LPSTR pszTerm = strchr(pszStart, pATF->cLineTerm);

         // If found and inside the read range terminate the string and the read.
         if (pszTerm && (pszTerm < pszStart+lMoveSize))
         {
            *pszTerm = '\0';
            lMoveSize = pszTerm - pszStart + 1;

            // When the counter gets decremented below, the loop will terminate.
            dwToRead = lMoveSize;
         }

         // Copy the data into the return buffer.
         strncpy(pszReturnBuf, pszStart, lMoveSize);
         pszReturnBuf[lMoveSize] = '\0';

         // Advance the buffer position
         pATF->lPos   += lMoveSize;
         dwToRead     -= lMoveSize;
         pszReturnBuf += lMoveSize;
      }
      else
      {
         // read another buffer if done with the current one:
         if (dwToRead > 0)    // ie - we arrived here because lBytesInBuf == 0
         {
            DWORD dwBytesRead;
            if (!ReadFile(pATF->hFile, pszReadBuf, pATF->lBufSize, &dwBytesRead, NULL))
               return GETS_ERROR;

            if (dwBytesRead == 0)
               return GETS_EOF;

            if (dwBytesRead != (DWORD)pATF->lBufSize)
               pATF->lBufReadLimit = dwBytesRead;
            else
               pATF->lBufReadLimit = pATF->lBufSize;
            pATF->lPos = 0;

            // Zero terminate the read block after the last byte read.
            // No bounds problem because we allocated the I/O buffer to be one byte
            // more than pATF->lBufSize.
            pszReadBuf[dwBytesRead] = '\0';

            // If the line terminator has not been set, set it now.
            if (pATF->cLineTerm == '\0')
               pATF->cLineTerm = GetLineTerminator(pszReadBuf);
         }
      }
   }

   // Take out the last character if it is '\r'.
   // (present if \r\n pairs terminate lines)
   int l = strlen(pszString);
   if (l && (pszString[l-1]=='\r'))
   {
      l--;
      pszString[l] = '\0';
   }
   
   return (DWORD(l) < dwBufSize-1) ? 0 : GETS_NOEOL;
}


//===============================================================================================
// FUNCTION: putsBuf
// PURPOSE:  Buffered version of puts.
// RETURNS:  ZERO          on success
//           GETS_EOF   on EOF
//           GETS_ERROR on error

int putsBuf(ATF_FILEINFO *pATF, LPCSTR pszString)
{
   WPTRASSERT(pATF);

   DWORD    dwBytes = strlen(pszString);
   DWORD    dwBytesWritten;

   // perform write if buffer size is 0:
   if (pATF->lBufSize == 0L)
      return WriteFile(pATF->hFile, pszString, dwBytes, &dwBytesWritten, NULL);

   // switch to write mode:
   if (pATF->bRead)
   {
      pATF->bRead = FALSE;
      pATF->lPos  = 0;
   }

   long  lBufSize    = pATF->lBufSize;
   char *pszWriteBuf = pATF->pszBuf;

   // determine free size left in buffer:
   long lFreeSize = lBufSize - pATF->lPos;
   ASSERT(lFreeSize > 0L);

   // move up to a single buffer
   long lMoveSize = min((DWORD)lFreeSize, dwBytes);
   memcpy(pszWriteBuf + pATF->lPos, pszString, lMoveSize);
   pATF->lPos += lMoveSize;

   // case 1:  doesn't fill buffer
   if (pATF->lPos < lBufSize)
      return TRUE;

   // write initial buffer - results handled in case 2 and 3:
   BOOL bReturn = WriteFile(pATF->hFile, pszWriteBuf, lBufSize, &dwBytesWritten, NULL);
   
   // case 2:  fills buffer, less than one buffer overflow (write one, move the rest)
   if (dwBytes - (DWORD)lMoveSize < (DWORD)lBufSize)
   {
      pATF->lPos = dwBytes - lMoveSize;
      if (pATF->lPos > 0L)
         memcpy(pszWriteBuf, pszString + lMoveSize, pATF->lPos);
      
      return bReturn;
   }

   // case 3:  multiple buffer's worth (write mem buffer, write the remainder, reset internals)
   if (bReturn)
      bReturn = WriteFile(pATF->hFile, pszString + lMoveSize, 
                          dwBytes - lMoveSize, &dwBytesWritten, NULL);
      
   pATF->lPos = 0L;
   return bReturn;
}
