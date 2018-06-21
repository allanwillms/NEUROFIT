//***********************************************************************************************
//
//    Copyright (c) 1994-2000 Axon Instruments.
//    All rights reserved.
//
//***********************************************************************************************
// HEADER:  CABFHDR.CPP
// PURPOSE: Contains member functions for the class CABFHeader. A class wrapper for the
//          ABFFileHeader structure.
// AUTHOR:  BHI  Mar 1994
//          PRC  Sep 1998
//

#include "wincpp.hpp"
#include "\AxonDev\Comp\AxABFFIO32\cabfhdr.hpp"
#include <math.h>
#include <time.h>
#include <limits.h>

//==============================================================================================
// FUNCTION: Initialize.
// PURPOSE:  Initialize all the fields of the base class structure to valid values.
//
void CABFHeader::Initialize()
{
   MEMBERASSERT();
   ABFH_Initialize(this);
}

//==============================================================================================
// FUNCTION: Constructor
// PURPOSE:  Initializes the object on creation.
//
CABFHeader::CABFHeader()
{
   MEMBERASSERT();
   Initialize();
}

//==============================================================================================
// FUNCTION:   Overloaded assignment operator
// PURPOSE:    Assignment from base class.
//
const CABFHeader& CABFHeader::operator=(const ABFFileHeader &FH)
{
   MEMBERASSERT();
   
   // Check we are working with a "new" header.
   ASSERT( ABFH_IsNewHeader( &FH ) );

   *(ABFFileHeader *)this = FH;
   return *this;
}

//==============================================================================================
// FUNCTION:   Overloaded copy constructor
// PURPOSE:    Copy constructor from base class.
//
CABFHeader::CABFHeader(const ABFFileHeader &FH)
{
   MEMBERASSERT();
   *this = FH;    // Let the overloaded operator=() function do the work.
}

//==============================================================================================
// FUNCTION:   Destructor
// PURPOSE:    Cleans up the object when it is destroyed.
//
CABFHeader::~CABFHeader()
{
   MEMBERASSERT();
}

//==============================================================================================
// FUNCTION: SetDateTime
// PURPOSE:  Sets the date and time for the header to the current date and time.
// NOTES:    Year is now (ABF v1.6) stored as a 4 digit year.
void CABFHeader::SetDateTime(UINT uStopWatch)
{
   MEMBERASSERT();

   SYSTEMTIME Time = { 0 };
   GetLocalTime(&Time);
   
   lFileStartDate      = long(Time.wYear*10000 + Time.wMonth*100 + Time.wDay);
   lFileStartTime      = long(((Time.wHour*60) + Time.wMinute)*60 + Time.wSecond);
   nFileStartMillisecs = short(Time.wMilliseconds);
   lStopwatchTime      = long(uStopWatch);
}

//==============================================================================================
// FUNCTION: SetADCName
// PURPOSE:  Set an ADC name from a C string.
//
void CABFHeader::SetADCName(UINT uChannel, LPCSTR ps)
{
   MEMBERASSERT();
   LPSZASSERT(ps);
   ASSERT(uChannel < ABF_ADCCOUNT);
   ABF_SET_STRING(sADCChannelName[uChannel], ps);
}

//==============================================================================================
// FUNCTION: GetADCName
// PURPOSE:  Return an ADC name as a C string.
//
void CABFHeader::GetADCName(UINT uChannel, char *ps, UINT uSize) const
{
   MEMBERASSERT();
   ARRAYASSERT(ps, uSize);
   ASSERT(uChannel < ABF_ADCCOUNT);
   ABF_GET_STRING(ps, sADCChannelName[uChannel], uSize);
   if (ps[0]=='\0')
      _snprintf(ps, uSize, "IN #%d", nADCPtoLChannelMap[uChannel]);
}

//==============================================================================================
// FUNCTION: SetMathUnits
// PURPOSE:  Set the math channel units from a C string.
//
void CABFHeader::SetMathUnits(LPCSTR ps)
{
   MEMBERASSERT();
   LPSZASSERT(ps);
   ABF_SET_STRING(sArithmeticUnits, ps);
}

//==============================================================================================
// FUNCTION: GetMathName
// PURPOSE:  Return the name of the math channel as a C string.
//
void CABFHeader::GetMathName(char *ps, UINT uSize) const
{
   MEMBERASSERT();
   ARRAYASSERT(ps, uSize);
   ABFH_GetMathChannelName(ps, uSize);
}

//==============================================================================================
// FUNCTION: GetMathUnits
// PURPOSE:  Return the math channel units as a C string.
//
void CABFHeader::GetMathUnits(char *ps, UINT uSize) const
{
   MEMBERASSERT();
   ARRAYASSERT(ps, uSize);
   ABF_GET_STRING(ps, sArithmeticUnits, uSize);
}

//==============================================================================================
// FUNCTION: SetADCUnits
// PURPOSE:  Set an ADC units from a C string.
//
void CABFHeader::SetADCUnits(UINT uChannel, LPCSTR ps)
{
   MEMBERASSERT();
   LPSZASSERT(ps);
   ASSERT(uChannel < ABF_ADCCOUNT);
   ABF_SET_STRING(sADCUnits[uChannel], ps);
}

//==============================================================================================
// FUNCTION: GetADCUnits
// PURPOSE:  Return an ADC units as a C string.
//
void CABFHeader::GetADCUnits(UINT uChannel, char *ps, UINT uSize) const
{
   MEMBERASSERT();
   ARRAYASSERT(ps, uSize);
   ASSERT(uChannel < ABF_ADCCOUNT);
   ABF_GET_STRING(ps, sADCUnits[uChannel], uSize);
}

//==============================================================================================
// FUNCTION: SetDACName
// PURPOSE:  Sets a DAC name from a C string.
//
void CABFHeader::SetDACName(UINT uChannel, LPCSTR ps)
{
   MEMBERASSERT();
   LPSZASSERT(ps);
   ASSERT(uChannel < ABF_DACCOUNT);
   ABF_SET_STRING(sDACChannelName[uChannel], ps);
}

//==============================================================================================
// FUNCTION: GetDACName
// PURPOSE:  Return a DAC name as a C string.
//
void CABFHeader::GetDACName(UINT uChannel, char *ps, UINT uSize) const
{
   MEMBERASSERT();
   ARRAYASSERT(ps, uSize);
   ASSERT(uChannel < ABF_DACCOUNT);
   ABF_GET_STRING(ps, sDACChannelName[uChannel], uSize);
   if (ps[0]=='\0')
      _snprintf(ps, uSize, "OUT #%u", uChannel);
}

//==============================================================================================
// FUNCTION: SetDACUnits
// PURPOSE:  Sets a DAC units from a C string.
//
void CABFHeader::SetDACUnits(UINT uChannel, LPCSTR ps)
{
   MEMBERASSERT();
   LPSZASSERT(ps);
   ABF_SET_STRING(sDACChannelUnits[uChannel], ps);
}

//==============================================================================================
// FUNCTION: GetDACUnits
// PURPOSE:  Return a DAC units as a C string.
//
void CABFHeader::GetDACUnits(UINT uChannel, char *ps, UINT uSize) const
{
   MEMBERASSERT();
   ARRAYASSERT(ps, uSize);
   ABF_GET_STRING(ps, sDACChannelUnits[uChannel], uSize);
}

//==============================================================================================
// FUNCTION: SetComment
// PURPOSE:  Set the comment string in the ABF header.
//
void CABFHeader::SetComment(LPCSTR ps)
{
   MEMBERASSERT();
   LPSZASSERT(ps);
   ABF_SET_STRING(sFileComment, ps);
}

//==============================================================================================
// FUNCTION: GetComment
// PURPOSE:  Get the comment string from the ABF header.
//
void CABFHeader::GetComment(char *ps, UINT uSize) const
{
   MEMBERASSERT();
   ARRAYASSERT(ps, uSize);
   ABF_GET_STRING(ps, sFileComment, uSize);
}

//==============================================================================================
// FUNCTION:   SetCreatorInfo
// PURPOSE:    Takes a zero terminated string and fits as much of it as it can in the
//             sCreatorInfo field of the ABF header, padding out with blanks if required.
// PARAMETERS:
//    pszCreator - zero terminated string containing creator information.
//
void CABFHeader::SetCreatorInfo(LPCSTR ps)
{
   MEMBERASSERT();
   LPSZASSERT(ps);
   ABF_SET_STRING(sCreatorInfo, ps);
}

//==============================================================================================
// FUNCTION:   GetCreatorInfo
// PURPOSE:    Returns the sCreatorInfo field of the ABF header, stripping blanks.
// PARAMETERS:
//    pszCreator - zero terminated string containing creator information.
//
void CABFHeader::GetCreatorInfo(char *ps, UINT uSize) const
{
   MEMBERASSERT();
   ARRAYASSERT(ps, uSize);
   ABF_GET_STRING(ps, sCreatorInfo, uSize);
}

//==============================================================================================
// FUNCTION:   SetUserList
// PURPOSE:    Sets the sParamValueList field of the ABF header, padding with blanks.
// PARAMETERS:
//    ps - zero terminated string containing the UserList information.
//
void CABFHeader::SetUserList( UINT uListNum, LPCSTR ps )
{
   MEMBERASSERT();
   LPSZASSERT(ps);
   ABF_SET_STRING(sULParamValueList[uListNum], ps);
}

   
//==============================================================================================
// FUNCTION:   GetUserList
// PURPOSE:    Returns the sParamValueList field of the ABF header, stripping blanks.
// PARAMETERS:
//    ps - zero terminated string containing the UserList information.
//
void CABFHeader::GetUserList( UINT uListNum, char *ps, UINT uSize ) const
{
   MEMBERASSERT();
   ARRAYASSERT(ps, uSize);
   ABF_GET_STRING(ps, sULParamValueList[uListNum], uSize);
}

   
//==============================================================================================
// FUNCTION:   SetProtocolPath
// PURPOSE:    Sets the sProtocolPath field of the ABF header, padding with blanks.
// PARAMETERS:
//    ps - zero terminated string containing the ProtocolPath information.
//
void CABFHeader::SetProtocolPath( LPCSTR ps )
{
   MEMBERASSERT();
   LPSZASSERT(ps);

   ABF_SET_STRING( sProtocolPath, ps) ;
}

//==============================================================================================
// FUNCTION:   SetDACFilePath
// PURPOSE:    Sets the sDACFilePath field of the ABF header, padding with blanks.
// PARAMETERS:
//    ps - zero terminated string containing the DACFilePath information.
//
void CABFHeader::SetDACFilePath(UINT uChannel, LPCSTR ps)
{
   MEMBERASSERT();
   LPSZASSERT(ps);
   ABF_SET_STRING(sDACFilePath[uChannel], ps);
}


//==============================================================================================
// FUNCTION:   GetDACFilePath
// PURPOSE:    Returns the sDACFilePath field of the ABF header, stripping blanks.
// PARAMETERS:
//    ps - zero terminated string containing the DACFilePath information.
//
void CABFHeader::GetDACFilePath(UINT uChannel, char *ps, UINT uSize) const
{
   MEMBERASSERT();
   ARRAYASSERT(ps, uSize);
   ABF_GET_STRING(ps, sDACFilePath[uChannel], uSize);
}


//==============================================================================================
// FUNCTION:   GetADCDisplayRange
// PURPOSE:    Calculates the upper and lower limits of the display given the display
//             amplification and offset in the ABF header for this ADC channel.
// PARAMETERS:
//    uChannel   - The physical ADC channel number to get the factors for.
//    pfUUTop    - Pointers to return locations for top and bottom of the display.
//    pfUUBottom
//
void CABFHeader::GetADCDisplayRange(UINT uChannel, float *pfUUTop, float *pfUUBottom) const
{
   // Validate parameters.
   MEMBERASSERT();
   ASSERT(uChannel < ABF_ADCCOUNT);
   WPTRASSERT(pfUUTop);
   WPTRASSERT(pfUUBottom);
   
   ABFH_GetADCDisplayRange(this, uChannel, pfUUTop, pfUUBottom);
}


//==============================================================================================
// FUNCTION:   GetADCFullScaleRange
// PURPOSE:    Calculates the upper and lower limits of full scale range of this
//             ADC channel.
// PARAMETERS:
//    uChannel   - The physical ADC channel number to get the factors for.
//    pfUUMax    - Pointers to return locations for max and min values.
//    pfUUMin
//
void CABFHeader::GetADCFullScaleRange(UINT uChannel, float *pfUUMax, float *pfUUMin) const
{
   // Validate parameters.
   MEMBERASSERT();
   ASSERT(uChannel < ABF_ADCCOUNT);
   WPTRASSERT(pfUUMax);
   WPTRASSERT(pfUUMin);
   float fMax = ADCtoUU(uChannel, short(lADCResolution-1));
   float fMin = ADCtoUU(uChannel, short(-lADCResolution));
   if (fMax < fMin)
   {
      float fTemp = fMax;
      fMax = fMin;
      fMin = fTemp;
   }
   *pfUUMax = fMax;
   *pfUUMin = fMin;
}


//==============================================================================================
// FUNCTION: SamplesToMS
// PURPOSE:  Converts a sample offset within an episode into ms since the start of the episode.
//
float CABFHeader::SamplesToMS(int nSamples) const
{
   UINT uClockChange = GetClockChange();

   if ((nOperationMode!=ABF_WAVEFORMFILE) || (nSamples <= int(uClockChange)))
      return nSamples * fADCSampleInterval / 1000.0F;
   
   return (uClockChange * fADCSampleInterval + 
           (nSamples-uClockChange) * fADCSecondSampleInterval) / 1000.0F;
}

//==============================================================================================
// FUNCTION:   SetThreshold
// PURPOSE:    Sets the threshold value, limiting the new value to the range of the A/D converter.
//
BOOL CABFHeader::SetThreshold(float fThreshold)
{
   MEMBERASSERT();
   if (!IsThresholdTrigger())
      return FALSE;
   fTriggerThreshold = ClipADCUUValue(GetTriggerChannel(), fThreshold);
   return (fTriggerThreshold==fThreshold);
}


//==============================================================================================
// FUNCTION:   Change threshold
// PURPOSE:    Changes the threshold value by a percentage of the full scale range, limiting
//             the calculated value to the range of the A/D converter.
// PARAMETERS:
//    fPercent - The percentage change requested.
//
BOOL CABFHeader::ChangeThreshold(float fPercent)
{
   MEMBERASSERT();
   if (!IsThresholdTrigger())
      return FALSE;
   float fADCToUUFactor, fADCToUUShift;
   GetADCtoUUFactors(GetTriggerChannel(), &fADCToUUFactor, &fADCToUUShift);

   float fUURange = (lADCResolution-1) * fADCToUUFactor;
   fTriggerThreshold += fUURange * fPercent/100.0F;
   
   // Limit the threshold level to values representable in 16 bit ADC values,
   // keeping one ADC count away from the absolute extremes of the range.
   float fUUMax = (lADCResolution-2) * fADCToUUFactor + fADCToUUShift;
   float fUUMin = (-lADCResolution+1)* fADCToUUFactor + fADCToUUShift;
   if (fUUMax < fUUMin)
   {
      float fTemp = fUUMax;
      fUUMax      = fUUMin;
      fUUMin      = fTemp;
   }
   if (fTriggerThreshold > fUUMax)
   {
      fTriggerThreshold = fUUMax;
      return FALSE;
   }
   if (fTriggerThreshold < fUUMin)
   {
      fTriggerThreshold = fUUMin;
      return FALSE;
   }
   return TRUE;
}

//==============================================================================================
// FUNCTION:   SetDACHolding
// PURPOSE:    Sets the holding level output on a D/A channel.
//
BOOL CABFHeader::SetDACHolding(UINT uDACChannel, float fDACHolding)
{
   MEMBERASSERT();
   if (uDACChannel >= ABF_DACCOUNT)
      return FALSE;
      
   float fDACToUUFactor, fDACToUUShift;
   GetDACtoUUFactors(uDACChannel, &fDACToUUFactor, &fDACToUUShift);

   // Limit the threshold level to values representable in 16 bit ADC values,
   // keeping one ADC count away from the absolute extremes of the range.
   float fUUTop    = (lDACResolution-2) * fDACToUUFactor + fDACToUUShift;
   float fUUBottom = (-lDACResolution+1)* fDACToUUFactor + fDACToUUShift;

   // Negative scaling factors will cause the "top" to be less than the "bottom"
   if (fUUTop < fUUBottom)
   {
      float fTemp = fUUBottom;
      fUUBottom   = fUUTop;
      fUUTop      = fTemp;
   }
   
   BOOL bRval = TRUE;
   if (fDACHolding > fUUTop)
   {
      fDACHolding = fUUTop;
      bRval = FALSE;
   }
   if (fDACHolding < fUUBottom)
   {
      fDACHolding = fUUBottom;
      bRval = FALSE;
   }
   fDACHoldingLevel[uDACChannel] = fDACHolding;
   return bRval;
}


//==============================================================================================
// FUNCTION:   CheckChannelNames
// PURPOSE:    Checks that all AI names are not empty.
//
BOOL CABFHeader::CheckChannelNames() const
{
   MEMBERASSERT();
   
   // Characters that are regarded as white space.
   static char szWhiteSpace[] = " \t\r\n";
   char szName[ABF_ADCNAMELEN+1];
   
   for (short i=0; i<nADCNumChannels; i++)
   {
      // Get a pointer to the next channel name.
      strncpy(szName, sADCChannelName[nADCSamplingSeq[i]], ABF_ADCNAMELEN);
      szName[ABF_ADCNAMELEN] = '\0';
      
      // Skip leading white space.
      char *p = szName + strspn(szName, szWhiteSpace);
      
      // If nothing left, return.
      if (*p=='\0')
         return FALSE;
   }
   return TRUE;
}


//==============================================================================================
// FUNCTION:   MultiplexOffset
// PURPOSE:    Returns an index into the sampling sequence that corresponds to the given ADC
//             channel.
// PARAMETERS:
//    uChannel - Physical channel number to look for.
// RETURNS:    > 0 - The position that the ADC channel was first found in the sampling sequence.
//             -1  - The ADC channel is not present in the sampling sequence.
//
int CABFHeader::MultiplexOffset(UINT uChannel) const
{
   MEMBERASSERT();
   for (int i=0; i<nADCNumChannels; i++)
      if (UINT(nADCSamplingSeq[i])==uChannel)
         return i;
   return ABF_UNUSED_CHANNEL;
}


//==============================================================================================
// FUNCTION:   MultiplexOffset
// PURPOSE:    Returns an index into the sampling sequence that corresponds to the given ADC
//             signal name.
// PARAMETERS:
//    uChannel - Physical channel number to look for.
// RETURNS:    > 0 - The position that the ADC channel was first found in the sampling sequence.
//             -1  - The ADC channel is not present in the sampling sequence.
//
int CABFHeader::MultiplexOffset(LPCSTR pszSignal) const
{
   MEMBERASSERT();
   char szName[ABF_ADCNAMELEN+1];
   for (int i=0; i<nADCNumChannels; i++)
   {
      GetADCName(nADCSamplingSeq[i], szName, sizeof(szName));
      if (strcmp(szName, pszSignal)==0)
         return i;
   }
   return ABF_UNUSED_CHANNEL;
}


//==============================================================================================
// FUNCTION:   CheckTriggerChannel
// PURPOSE:    Checks that the value set in the nTriggerSource parameter is valid.
//
BOOL CABFHeader::CheckTriggerChannel() const
{
   MEMBERASSERT();
   if (nOperationMode==ABF_WAVEFORMFILE)
      return TRUE;
   if (!IsThresholdTrigger())
      return TRUE;
   return ( MultiplexOffset( GetTriggerChannel() ) != ABF_UNUSED_CHANNEL );
}


//==============================================================================================
// FUNCTION: GetMxTriggerOffset
// PURPOSE:  Get the multiplexed offset to the data channel that holds the data to be triggered off.      
//
UINT CABFHeader::GetMxTriggerOffset() const
{
   MEMBERASSERT();
   if (!IsThresholdTrigger())
      return nADCNumChannels-1;
   return MultiplexOffset(GetTriggerChannel());
}


//==============================================================================================
// FUNCTION: GetADCTriggerPolarity
// PURPOSE:  Convert the trigger polarity WRT UserUnits to polarity WRT ADC units.      
//
BOOL CABFHeader::GetADCTriggerPolarity() const
{
   MEMBERASSERT();
   if (IsThresholdTrigger())
   {      
      float fADCToUUFactor, fADCToUUShift;
      GetADCtoUUFactors(GetTriggerChannel(), &fADCToUUFactor, &fADCToUUShift);
      if (fADCToUUFactor < 0)
         return !nTriggerPolarity;
   }
   return nTriggerPolarity;
}
   

//==============================================================================================
// FUNCTION: GetHoldingDuration
// PURPOSE:  Returns the number of waveform points to be set to holding level at the start and
//           end of each episode.
//
UINT CABFHeader::GetHoldingDuration() const
{
   MEMBERASSERT();
   return ABFH_GetHoldingDuration(this);
}
   

//==============================================================================================
// FUNCTION: GetVoltstoUUFactors
// PURPOSE:  Calculates conversion factors to go from Volts (at the ADC input) to UserUnits.
//
void CABFHeader::GetVoltstoUUFactors(UINT uChannel, float *pfVoltToUUFactor, float *pfVoltToUUShift) const
{
   MEMBERASSERT();
   float fScaleFactor = fInstrumentScaleFactor[uChannel];

   // FIX FIX FIX PRC DEBUG Telegraph changes - check !
   if( nTelegraphEnable[uChannel] )
      fScaleFactor *= fTelegraphAdditGain[uChannel];

   if (nSignalType != 0)
      fScaleFactor *= fSignalGain[uChannel];

   ASSERT(fScaleFactor != 0.0F);
   if (fScaleFactor==0.0F)
      fScaleFactor = 1.0F;

   float fOffset = fInstrumentOffset[uChannel];
   if (nSignalType != 0)
      fOffset -= fSignalOffset[uChannel];

   *pfVoltToUUFactor = 1.0F / fScaleFactor;
   *pfVoltToUUShift  = fOffset;
   // UU = Volts * fVoltToUUFactor + fVoltToUUShift;
}


//==============================================================================================
// FUNCTION: CheckMathChannels
// PURPOSE:  Checks that the channels selected for combination as a math channel are both in the
//           acquisition sequence.
//
BOOL CABFHeader::CheckMathChannels() const
{
   MEMBERASSERT();
   return ( MultiplexOffset( nArithmeticADCNumA ) != ABF_UNUSED_CHANNEL) &&
          ( MultiplexOffset( nArithmeticADCNumB ) != ABF_UNUSED_CHANNEL);
}


//==============================================================================================
// FUNCTION: CheckMathExpression
// PURPOSE:  Checks that the parameters in the math channel expression are sensible.
//
BOOL CABFHeader::CheckMathExpression() const
{
   MEMBERASSERT();
   // Check that the display range is sensible.
   if (fArithmeticUpperLimit == fArithmeticLowerLimit)
      return FALSE;

   // Check for division by zero.
   if (sArithmeticOperator[0]=='/')
   {
      if (fArithmeticK3==0.0F && fArithmeticK4==0.0F)
         return FALSE;
   }
   return TRUE;
}


//==============================================================================================
// FUNCTION: CheckSampleIntervals
// PURPOSE:  Checks that the sample intervals in the header are valid.
//
BOOL CABFHeader::CheckSampleIntervals(float fClockResolution) const
{
   return ABFH_CheckSampleIntervals(this, fClockResolution, NULL);
}


//==============================================================================================
// Table of sample intervals, their assigned oversampled "raw interval" and the expansion factor.
//==============================================================================================
// Interval       <20  25   30   35   40   45   50   55   60   65   70   75   80   85   90   95   100
// Raw Interval    "  12.5 15.0 17.5 20.0 15.0 12.5 11.0 12.0 13.0 14.0 15.0 16.0 17.0 18.0 19.0 20.0
// Divisor         1    2    2    2    2    3    4    5    5    5    5    5    5    5    5    5    5

// Interval       105  110  115  120  125  130  135  140  145  150  155  160  165  170  175  180
// Raw Interval   15.0 11.0 11.5 12.0 12.5 13.0 13.5 14.0 14.5 15.0 15.5 16.0 16.5 17.0 17.5 18.0
// Divisor          7   10   10   10   10   10   10   10   10   10   10   10   10   10   10   10

// Interval       185  190  195  200  210  220  230  240  250  260  270  280  290  300  310  320
// Raw Interval   18.5 19.0 19.5 20.0 15.0 11.0 11.5 12.0 12.5 13.0 13.5 14.0 14.5 15.0 15.5 16.0
// Divisor         10   10   10   10   14   20   20   20   20   20   20   20   20   20   20   20

// Interval       330  240  350  360  370  380  390  400  >400
// Raw Interval   16.5 17.0 17.5 28.0 18.5 19.0 19.5 20.0 20.0
// Divisor         20   20   20   20   20   20   20   20   20 

inline int _Divisor(const double dMinInterval)
{
   // If less than 20us allow full board resolution.
   if (dMinInterval <= 20.0)
      return 1;

   // >20us but <=200us must be multiple of 5us.
   int nMinInterval = int(dMinInterval + 0.5);
   if (nMinInterval <= 40)
      return 2;
   switch (nMinInterval)
   {
      case 45:
         return 3;
      case 50:
         return 4;
      case 105:
         return 7;
      case 210:
         return 14;
   }
   if (nMinInterval <= 100)
      return 5;
   if (nMinInterval <= 200)
      return 10;

   // >200us but <=400us must be multiple of 10us.
   // >400us must be multiple of 20us.
   // Either way use multiples of 20us
   // This assumes that the digitizer resolution is at least 0.5us
   return 20;
}

//==============================================================================================
// FUNCTION:   GetRawSampleInterval
// PURPOSE:    Get the sample interval to use as the underlying sample interval for the acquisition.
// NOTES:      The assumption on which the following code is based is that the underlying sample 
//             interval must be kept in the range 10 us - 20 us (except for sub-10 us acquisitions)
//             in order for the host computer to support concurrent AI/AO and still have enough
//             bus and CPU bandwidth to do other useful work.
//
float CABFHeader::GetRawSampleInterval() const
{
   MEMBERASSERT();
   ASSERT(CheckSampleIntervals(0.5F));
   if (nOperationMode!=ABF_WAVEFORMFILE)
      return fADCSampleInterval;
   
   double dMinInterval = fADCSampleInterval;
   if ((fADCSecondSampleInterval != 0.0F) && 
       (fADCSecondSampleInterval < fADCSampleInterval))
      dMinInterval = fADCSecondSampleInterval;

   int nDivisor = _Divisor(dMinInterval);
   return float(dMinInterval / nDivisor);
}


//==============================================================================================
// FUNCTION:   GetRawEpisodeLength
// PURPOSE:    Get the length of an episode in "Raw" samples.
//
UINT CABFHeader::GetRawEpisodeLength() const
{
   MEMBERASSERT();
   float fRawSampleInterval = GetRawSampleInterval();

   UINT uLength = 0;   
   if ((nOperationMode==ABF_WAVEFORMFILE) &&
       (fADCSecondSampleInterval != 0.0F))
   {
      UINT uClockChange      = GetClockChange();
      UINT uFirstDecimation  = UINT(fADCSampleInterval / fRawSampleInterval + 0.5F);
      UINT uSecondDecimation = UINT(fADCSecondSampleInterval / fRawSampleInterval + 0.5F);
      uLength = uClockChange * uFirstDecimation + UINT(lNumSamplesPerEpisode - uClockChange) * uSecondDecimation;
   }
   else
   {
      UINT uDecimation  = UINT(fADCSampleInterval / fRawSampleInterval + 0.5F);
      uLength = UINT(lNumSamplesPerEpisode) * uDecimation;
   }
   return uLength;
}


//==============================================================================================
// FUNCTION: GetFirstHoldingLevel
// PURPOSE:  Gets the holding level at the start of a Waveform episode.
//
float CABFHeader::GetFirstHoldingLevel( UINT uDAC ) const
{
   ASSERT( uDAC < ABF_WAVEFORMCOUNT );

   // No conditioning present or conditioning on different channel
   if ((nConditEnable[uDAC] == 0) || 
       (fPostTrainPeriod[uDAC] == 0.0F))
      return fDACHoldingLevel[uDAC];
   if (fPostTrainPeriod[uDAC] > 0.0F)
      return fPostTrainLevel[uDAC];
   return fBaselineLevel[uDAC];
}

//==============================================================================================
// FUNCTION: GetClockChange
// PURPOSE:  Gets the point at which the sampling interval changes if split clock.
//
UINT CABFHeader::GetClockChange() const
{
   MEMBERASSERT();
   return ABFH_GetClockChange(this);
}


//==============================================================================================
// FUNCTION: GetNumActiveDACChannels
// PURPOSE:  Calculate the number of active DAC channels in the output sequence.
//           This will be less than or equal to the output sequence length.
// NOTES:    The actual number of output may be padded with extra holding channels to make
//           the input sequence length an integer multiple of the output sequence length.
//
UINT CABFHeader::GetNumActiveDACChannels() const
{
   MEMBERASSERT();
   if (nOperationMode != ABF_WAVEFORMFILE)
      return 0;

   UINT uDACChannels = 0;
   for( int i=0; i<ABF_WAVEFORMCOUNT; i++ )
   {
      if( nWaveformEnable[i] &&
          nWaveformSource[i]!=ABF_WAVEFORMDISABLED)
         uDACChannels++;
      else if (nConditEnable[i])
         uDACChannels++;
   }
   return uDACChannels;
}

//==============================================================================================
// FUNCTION: GetDACSamplingSeq
// PURPOSE:  Get the sampling sequence for the output data stream.
// PARAMETERS:
//    bSeparateDigOut - TRUE if synchronous DigitalOUTs have a separate output channel.
//                      FALSE if synchronous DigitalOUTs are folded into the low 4 bits of the DACs
// RETURNS:  Number of items in the list. 
// NOTES:    The return value may not necessarily be the same as the value returned by
//           GetNumActiveDACChannels() because of the digital output channel.
//           The actual number of output may be padded with extra holding channels to make
//           the input sequence length an integer multiple of the output sequence length.
//
UINT CABFHeader::GetDACSamplingSeq(short *pnSamplingSeq, UINT uSeqSize, BOOL bSeparateDigOut) const
{
   MEMBERASSERT();
   if (nOperationMode != ABF_WAVEFORMFILE)
      return 0;

   UINT uDACChannels = 0;
   short anDACChannels[ABF_ADCCOUNT] = { 0 };

   // First add the waveform channels & conditioning channel
   for( int i=0; i<ABF_WAVEFORMCOUNT; i++ )
   {
      if( ((nWaveformEnable[i] && (nWaveformSource[i]!=ABF_WAVEFORMDISABLED)) || (nConditEnable[i])) )
         anDACChannels[uDACChannels++] = (short)i;
   }

   // Lastly add in the Digital Out channel
   if (nDigitalEnable)
   {
      if (bSeparateDigOut)
      {
         anDACChannels[uDACChannels++] = ABF_DIGITAL_OUT_CHANNEL;

         // Pad to the ADC sequence length.
         while (uDACChannels < UINT(nADCNumChannels))
            anDACChannels[uDACChannels++] = ABF_PADDING_OUT_CHANNEL;  // Any channel will do.
      }
      else if (uDACChannels==0)
         anDACChannels[uDACChannels++] = nActiveDACChannel;  // The channel the digital outs are assigned to.
   }

   // If the user wants the sequence, fill in what will fit.
   if (pnSamplingSeq)
   {
      WARRAYASSERT(pnSamplingSeq, uSeqSize);
      ASSERT(uSeqSize >= uDACChannels);
      for (UINT i=0; i<min(uDACChannels, uSeqSize); i++)
         pnSamplingSeq[i] = anDACChannels[i];
   }
   return uDACChannels;
}

//==============================================================================================
// FUNCTION: GetOutputSequenceLength
// PURPOSE:  Calculate the number of output channels that are required in the output sequence.
// PARAMETERS:
//    bSeparateDigOut - TRUE if synchronous DigitalOUTs have a Separate output channel.
//                      FALSE if synchronous DigitalOUTs are folded into the low 4 bits of the DACs
// NOTES:    The actual number of output may be padded with extra holding channels to make
//           the input sequence length an integer multiple of the output sequence length.
//
UINT CABFHeader::GetOutputSequenceLength(BOOL bSeparateDigOut) const
{
   MEMBERASSERT();
   return GetDACSamplingSeq(NULL, 0, bSeparateDigOut);
}

//==============================================================================================
// FUNCTION: GetEpisodeDuration
// PURPOSE:  Gets the duration of the Waveform Episode (in us), allowing for split clock etc.
//
double CABFHeader::GetEpisodeDuration() const
{
   MEMBERASSERT();
   double dEpisodeDuration = 0.0;
   ABFH_GetEpisodeDuration(this, &dEpisodeDuration);
   return dEpisodeDuration;
}


//==============================================================================================
// FUNCTION: GetPNDuration
// PURPOSE:  Gets the duration of a P/N sequence (in us), including settling times.
//
double CABFHeader::GetPNDuration( UINT uChannel) const
{
   MEMBERASSERT();
   double dPNDuration = 0.0;
   ABFH_GetPNDurationEx(this, uChannel, &dPNDuration);
   return dPNDuration;
}


//==============================================================================================
// FUNCTION: GetTrainDuration
// PURPOSE:  Gets the duration of a conditioning train in us.
//
double CABFHeader::GetTrainDuration( UINT uChannel ) const
{
   MEMBERASSERT();
   double dTrainDuration = 0.0;
   ABFH_GetTrainDurationEx( this, uChannel, &dTrainDuration );
   return dTrainDuration;
}

//==============================================================================================
// FUNCTION: GetMetaEpisodeDuration
// PURPOSE:  Gets the duration of a whole meta-episode (in us).
//
double CABFHeader::GetMetaEpisodeDuration() const
{
   MEMBERASSERT();
   double dMetaEpisodeDuration = 0.0;
   ABFH_GetMetaEpisodeDuration(this, &dMetaEpisodeDuration);
   return dMetaEpisodeDuration;
}

//==============================================================================================
// FUNCTION: GetEpisodeStartToStart
// PURPOSE:  Gets the start to start period for the episode in us.
//
double CABFHeader::GetEpisodeStartToStart() const
{
   MEMBERASSERT();
   double dEpisodeStartToStart = 0.0;
   ABFH_GetEpisodeStartToStart(this, &dEpisodeStartToStart);
   return dEpisodeStartToStart;
}

//==============================================================================================
// FUNCTION: GetRunDuration
// PURPOSE:  Gets the start to start period for the run in us.
//
double CABFHeader::GetRunDuration() const
{
   MEMBERASSERT();
   return GetEpisodeStartToStart() * (lEpisodesPerRun-1) + GetMetaEpisodeDuration();
}

//==============================================================================================
// FUNCTION: GetRunStartToStart
// PURPOSE:  Gets the start to start period for the run in us.
//
double CABFHeader::GetRunStartToStart() const
{
   MEMBERASSERT();
   double dRval = GetRunDuration();
   return max(dRval, double(fRunStartToStart)*1E6F);
}

//==============================================================================================
// FUNCTION: GetTrialSamples
// PURPOSE:  The number of raw samples that make up the trial.
//
UINT CABFHeader::GetRawTrialSamples() const
{
   MEMBERASSERT();
   double dTrialSamples = 0.0;
   if (nOperationMode==ABF_WAVEFORMFILE)
   {
      // If the trial is externally triggered we have no way of calculating the number of
      // samples that it will take to complete it, so we just check for what we can in this case.
      double dTrialDuration = 0.0;
      if ( !IsThresholdTrigger() || 
         (nULEnable[0] && (nULParamToVary[0]==ABF_EPISODESTARTTOSTART)) ||
         (nULEnable[1] && (nULParamToVary[1]==ABF_EPISODESTARTTOSTART)) )
      {
         dTrialDuration = double(lEpisodesPerRun) * GetMetaEpisodeDuration();
      }
      else
      {
         int nRunsPerTrial  = max(lRunsPerTrial, 1);
         dTrialDuration = (nRunsPerTrial-1) * GetRunStartToStart() + GetRunDuration();
      }
      dTrialSamples = dTrialDuration / GetRawSampleInterval();
   }
   else if (nOperationMode==ABF_HIGHSPEEDOSC)
      dTrialSamples = double(lEpisodesPerRun) * lNumSamplesPerEpisode;
   else
      dTrialSamples = double(fSecondsPerRun)*1E6 / fADCSampleInterval;

   // If "continuous" set the number of samples to one less than the absolute max.
   if (dTrialSamples <= 0.0)
      dTrialSamples = INT_MAX-1;

   // Limit the return value to the range of the UINT (32-bit of course).
   return UINT(min(double(UINT_MAX), floor(dTrialSamples+0.5)));
}


//==============================================================================================
// FUNCTION: GetFastInterval
// PURPOSE:  Return the fastest of the two sample intervals.
//
float CABFHeader::GetFastInterval() const
{
   MEMBERASSERT();
   if (nOperationMode!=ABF_WAVEFORMFILE)
      return fADCSampleInterval;

   float fSecondInterval = fADCSecondSampleInterval;
   if (fSecondInterval == 0.0F)
      fSecondInterval = fADCSampleInterval;
   return min(fADCSampleInterval, fSecondInterval);
}


//==============================================================================================
// FUNCTION: SetFastInterval
// PURPOSE:  Set the fast interval to a new value, retaining the ratio with the slow interval.
//
void CABFHeader::SetFastInterval(float fFastInterval)
{
   MEMBERASSERT();
   if ((nOperationMode!=ABF_WAVEFORMFILE) ||
       (fADCSecondSampleInterval==0.0F))
   {
      fADCSampleInterval = fFastInterval;
      return;
   }

   if (fADCSampleInterval < fADCSecondSampleInterval)
   {
      fADCSecondSampleInterval *= fFastInterval / fADCSampleInterval;
      fADCSampleInterval        = fFastInterval;
   }
   else
   {
      fADCSampleInterval      *= fFastInterval / fADCSecondSampleInterval;
      fADCSecondSampleInterval = fFastInterval;
   }
}


//===============================================================================================
// FUNCTION: SetAutoAnalysis
// PURPOSE:  Sets the auto analysis parameters in the ABF header.
//
void CABFHeader::SetAutoAnalysis( int nEnable, LPCSTR pszMacroName )
{
   MEMBERASSERT();
   
   ASSERT( nEnable >= ABF_AUTOANALYSE_DISABLED );
   ASSERT( nEnable <= ABF_AUTOANALYSE_DEFAULT );

   nAutoAnalyseEnable = (short)nEnable;

   if( nEnable == ABF_AUTOANALYSE_RUNMACRO)
   {
      LPSZASSERT( pszMacroName );
      ABF_SET_STRING( sAutoAnalysisMacroName, pszMacroName);
   }
}

//===============================================================================================
// FUNCTION: GetAutoAnalysis
// PURPOSE:  Gets the auto analysis parameters from the ABF header.
//
int CABFHeader::GetAutoAnalysis( char *pszMacroName, UINT uSize ) const
{
   MEMBERASSERT();
   
   if( nAutoAnalyseEnable == ABF_AUTOANALYSE_RUNMACRO )
   {
      ARRAYASSERT(pszMacroName, uSize);
      ABF_GET_STRING( pszMacroName, sAutoAnalysisMacroName, uSize );
   }
      
   return nAutoAnalyseEnable;
}

//===============================================================================================
// FUNCTION: operator==
// PURPOSE:  Compares two CABFHeaders, returns TRUE if they are the same.
//
BOOL CABFHeader::operator==(CABFHeader &FH)
{
   MEMBERASSERT();
   
   BOOL bSame = ( memcmp( &FH, this, sizeof( CABFHeader ) ) == 0 );

#if _DEBUG
   // For debug builds, if the headers are different log the point where they differ.
   if( !bSame )
   {
      char *pThis = (char *)this;
      char *pThat = (char *)&FH;
      BOOL bOverride = TRUE;
      BOOL bInclude  = TRUE;

      // Elements we know may differ.
      UINT uExcludeList[] = { 20, 21, 22, 23, 24, 25, 28, 190, 366, 367 };

      for( UINT i=0; i<sizeof(ABFFileHeader); i++ )
      {
         if( pThis[i] != pThat[i] )
         {
            // See if it is in the Exclude List
            for( UINT j=0; j<ELEMENTS_IN(uExcludeList); j++ )
            {
               if( i == uExcludeList[j] )
               {
                  bInclude = FALSE;
                  break;
               }
            }

            if( bInclude && ( i < sizeof(ABFFileHeader) ) )
            {
               bOverride = FALSE;
               TRACE1( "Headers differ at byte %d.\n", i );
            }
         }
      }

      // If the differences were all in the Exclude List, then treat the 
      // headers as being the same.
      if( bOverride )
         bSame = TRUE;
   }
#endif   // _DEBUG

   return bSame;
}
