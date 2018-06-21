//**********************************************************************************************
//
//    Copyright (c) 1997 - 1998 Axon Instruments.
//    All rights reserved.
//
//**********************************************************************************************
// MODULE:  UserList.CPP
// PURPOSE: Contains class implimentation for the CUserList class.
// AUTHOR:  BHI  May 1997
//          PRC  Aug 1998
//

#include "wincpp.hpp"
#include "UserList.hpp"
#include "\AxonDev\Comp\AxABFFIO32\ABFUTIL.H"

//==============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Object initialization.
//
CUserList::CUserList()
{
   MEMBERASSERT();
   m_bEnable      = FALSE;
   m_nParamToVary = 0;
   m_uListEntries = 0;
   m_nListEpoch   = -1;
   memset(m_aItems, 0, sizeof(m_aItems));
}

//==============================================================================================
// FUNCTION: GetActiveEpoch
// PURPOSE:  Returns the index of the active epoch.
//
int CUserList::GetActiveEpoch() const
{
   MEMBERASSERT();
   return m_nListEpoch;
}

//==============================================================================================
// FUNCTION: SetItem
// PURPOSE:  Add an item into the list of unions in a form appropriate to the 
//           parameter-to-vary.
//
void CUserList::SetItem(UINT uIndex, LPCSTR pszItem)
{
   MEMBERASSERT();
   ASSERT(m_nParamToVary >= 0);
   ASSERT(m_nParamToVary < ABF_EPOCHINITDURATION+ABF_EPOCHCOUNT);

   switch (m_nParamToVary)
   {
      case ABF_CONDITNUMPULSES:
      case ABF_PNNUMPULSES:
         SetInteger(uIndex, pszItem);
         break;
         
      case ABF_CONDITBASELINEDURATION:
      case ABF_CONDITBASELINELEVEL:
      case ABF_CONDITSTEPDURATION:
      case ABF_CONDITSTEPLEVEL:
      case ABF_CONDITPOSTTRAINDURATION:
      case ABF_CONDITPOSTTRAINLEVEL:
      case ABF_EPISODESTARTTOSTART:
      case ABF_INACTIVEHOLDING:
         SetFloat(uIndex, pszItem);
         break;

      case ABF_DIGITALHOLDING:
         SetBinary(uIndex, pszItem);
         break;
         
      default:
         if (m_nParamToVary >= ABF_EPOCHINITDURATION)
            SetInteger(uIndex, pszItem);
         else if (m_nParamToVary >= ABF_EPOCHINITLEVEL)
            SetFloat(uIndex, pszItem);
         else if (m_nParamToVary >= ABF_PARALLELVALUE)
            SetBinary(uIndex, pszItem);
         break;
   }
}

//==============================================================================================
// FUNCTION: GetItem
// PURPOSE:  Retrieve an item from the list of unions in a form appropriate to the 
//           parameter-to-vary.
//
     
UserListItem CUserList::GetItem( UINT uEpisode, BOOL *pbIsInteger ) const
{
   MEMBERASSERT();

   ASSERT(m_nParamToVary >= 0);
   ASSERT(m_nParamToVary < ABF_EPOCHINITDURATION+ABF_EPOCHCOUNT);

   UserListItem ULItem;

   // Default to integer.
   BOOL bIsInteger = TRUE;

   UINT uEntry = Episode2Entry( uEpisode );

   switch (m_nParamToVary)
   {
      case ABF_CONDITNUMPULSES:
      case ABF_PNNUMPULSES:
      case ABF_DIGITALHOLDING:
         ULItem.n = GetInteger(uEntry);
         break;
         
      case ABF_CONDITBASELINEDURATION:
      case ABF_CONDITBASELINELEVEL:
      case ABF_CONDITSTEPDURATION:
      case ABF_CONDITSTEPLEVEL:
      case ABF_CONDITPOSTTRAINDURATION:
      case ABF_CONDITPOSTTRAINLEVEL:
      case ABF_EPISODESTARTTOSTART:
      case ABF_INACTIVEHOLDING:
         ULItem.f     = GetFloat(uEntry);
         bIsInteger = FALSE;
         break;

      default:
         if (m_nParamToVary >= ABF_EPOCHINITDURATION)
            ULItem.n = GetInteger(uEntry);
         else if (m_nParamToVary >= ABF_EPOCHINITLEVEL)
         {
            ULItem.f     = GetFloat(uEntry);
            bIsInteger = FALSE;
         }
         else if (m_nParamToVary >= ABF_PARALLELVALUE)
            ULItem.n = GetInteger(uEntry);
         break;
   }

   if( pbIsInteger )
      *pbIsInteger = bIsInteger;

   return ULItem;
}

//==============================================================================================
// FUNCTION: Initialize
// PURPOSE:  Parse the user list and build the array of unions that it corresponds to.
//
void CUserList::Initialize(const ABFFileHeader *pFH, UINT uListNum)
{
   MEMBERASSERT();
   ABFH_ASSERT(pFH);

   // Initialize all internals.
   m_nParamToVary = 0;
   m_uListEntries = 0;
   m_nListEpoch   = -1;
   memset(m_aItems, 0, sizeof(m_aItems));
   m_uListNum     = uListNum;
   m_bEnable = (pFH->nULEnable[m_uListNum] != 0);

   // If the user list is disabled, we are done.
   if (!m_bEnable)
      return;

   // Chack that the parameter to vary is OK.
   m_nParamToVary = pFH->nULParamToVary[m_uListNum];
   ASSERT(m_nParamToVary >= 0);
   ASSERT(m_nParamToVary < ABF_EPOCHINITDURATION+ABF_EPOCHCOUNT);

   // If the user list applies to an Epoch save the epoch index.
   if (m_nParamToVary >= ABF_EPOCHINITDURATION)
      m_nListEpoch = m_nParamToVary - ABF_EPOCHINITDURATION;
   else if (m_nParamToVary >= ABF_EPOCHINITLEVEL)
      m_nListEpoch = m_nParamToVary - ABF_EPOCHINITLEVEL;
   else if (m_nParamToVary >= ABF_PARALLELVALUE)
      m_nListEpoch = m_nParamToVary - ABF_PARALLELVALUE;

   // Take a local copy of the user list and zero terminate it.
   char szUserList[ABF_USERLISTLEN+1];
   ABF_GET_STRING(szUserList, pFH->sULParamValueList[m_uListNum], sizeof(szUserList));

   // Parse out the entries and put them in the array of unions.
   char *pszItem = strtok(szUserList, ", ");
   while (pszItem && *pszItem)
   {
      SetItem(m_uListEntries++, pszItem);
      pszItem = strtok(NULL, ", ");
   }

   // If nothing was found, make the list contain a single zero item.
   if (m_uListEntries==0)
      SetItem(m_uListEntries++, "");
}

//==============================================================================================
// FUNCTION: ClipToShort
// PURPOSE:  Converts an int or float to a short, clipping to the extremes.
//
inline short ClipToShort(int n)
{
   if (n>ADC_MAX)
      return ADC_MAX;
   if (n<ADC_MIN)
      return ADC_MIN;
   return short(n);
}

//==============================================================================================
// FUNCTION: Episode2Entry
// PURPOSE:  Convert an Episode number to an Entry number.
//
UINT CUserList::Episode2Entry(UINT uEpisode) const
{
   MEMBERASSERT();

   // If the list is empty, use the first entry.
   if( m_uListEntries == 0)
      return 0;

   // All episodes after the last one use the last entry in the user list.
   uEpisode = min(uEpisode, m_uListEntries);

   // Episodes are 1 based, Entry's are zero based.
   UINT uEntry = uEpisode - 1;

   return uEntry;
}

//==============================================================================================
// FUNCTION: UpdateHeader
// PURPOSE:  Update the header with the entry out of the User List for the given episode.
//
void CUserList::UpdateHeader(ABFFileHeader *pFH, UINT uDAC, UINT uEpisode) const
{
   MEMBERASSERT();
   ABFH_WASSERT(pFH);

   if (!m_bEnable)
      return;
   
   UINT uEntry = Episode2Entry( uEpisode );

   switch (m_nParamToVary)
   {
      case ABF_CONDITNUMPULSES:
         pFH->lConditNumPulses[uDAC] = GetInteger(uEntry);
         break;
         
      case ABF_CONDITBASELINEDURATION:
         pFH->fBaselineDuration[uDAC] = GetFloat(uEntry);
         break;
         
      case ABF_CONDITBASELINELEVEL:
         pFH->fBaselineLevel[uDAC] = GetFloat(uEntry);
         break;
         
      case ABF_CONDITSTEPDURATION:
         pFH->fStepDuration[uDAC] = GetFloat(uEntry);
         break;
         
      case ABF_CONDITSTEPLEVEL:
         pFH->fStepLevel[uDAC] = GetFloat(uEntry);
         break;
         
      case ABF_CONDITPOSTTRAINDURATION:
         pFH->fPostTrainPeriod[uDAC] = GetFloat(uEntry);
         break;
         
      case ABF_CONDITPOSTTRAINLEVEL:
         pFH->fPostTrainLevel[uDAC] = GetFloat(uEntry);
         break;
         
      case ABF_EPISODESTARTTOSTART:
         pFH->fEpisodeStartToStart = GetFloat(uEntry);
         break;
         
      case ABF_INACTIVEHOLDING:
         pFH->fDACHoldingLevel[1-uDAC] = GetFloat(uEntry);
         break;
         
      case ABF_DIGITALHOLDING:
         pFH->nDigitalHolding = ClipToShort(GetInteger(uEntry));
         break;
         
      case ABF_PNNUMPULSES:
         pFH->nPNNumPulses = ClipToShort(GetInteger(uEntry));
         break;
         
      default:
         ASSERT(m_nListEpoch >= 0);
         ASSERT(m_nListEpoch < ABF_EPOCHCOUNT);

         if (pFH->nULParamToVary[uDAC] >= ABF_EPOCHINITDURATION)
            pFH->lEpochInitDuration[uDAC][m_nListEpoch] = GetInteger(uEntry);
         else if (pFH->nULParamToVary[uDAC] >= ABF_EPOCHINITLEVEL)
            pFH->fEpochInitLevel[uDAC][m_nListEpoch] = GetFloat(uEntry);
         else if (pFH->nULParamToVary[uDAC] >= ABF_PARALLELVALUE)
            pFH->nDigitalValue[m_nListEpoch] = ClipToShort(GetInteger(uEntry));
         break;
   }    
   
}

//==============================================================================================
// FUNCTION: GetNumEntries
// PURPOSE:  Returns the number of entries in the list.
//
UINT CUserList::GetNumEntries() const
{
   MEMBERASSERT();
   return m_uListEntries;
}
