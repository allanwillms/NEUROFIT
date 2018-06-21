//***********************************************************************************************
//
//    Copyright (c) 1994-1998 Axon Instruments.
//    All rights reserved.
//
//***********************************************************************************************
// HEADER:  CABFHDR.HPP
// PURPOSE: Contains class definition for CABFHeader class. A class wrapper for the
//          ABFFileHeader structure used in Axon Binary Files.
// AUTHOR:  BHI  Mar 1994
//          PRC  Sep 1998
//

#ifndef INC_CABFHDR_HPP
#define INC_CABFHDR_HPP
#include "\AxonDev\Comp\common\axodefn.h"
#include "\AxonDev\Comp\common\axodebug.h"
#include "\AxonDev\Comp\AxABFFIO32\abfheadr.h"
#include "\AxonDev\Comp\AxABFFIO32\abfutil.h"

//==============================================================================================
// CLASS:   CABFHeader.
// PURPOSE: Simple wrapper for the ABFFileHeader structure, building on the functionality
//          exported by ABFHEADR.CPP - part of the Axon File Support Pack.
// NOTES:   This class requires either a static or dynamic linkage to the ABFH_xxx functions
//          provided by ABFHEADR.CPP. This can be in the form of a dynamic link to the AxABFFIO32.DLL
//          dynamic link library, a static link to the ABFLIB.LIB static library, or by including
//          the module ABFHEADR.CPP as part of the project/make file.
//
class CABFHeader : public ABFFileHeader
{
private:    // Data structures

public:     // Public member functions

   // Constructor. Initializes the object.
   CABFHeader();
   
   // // Copy constructor - use default.
   // CABFHeader(CABFHeader &FH);

   // Assignment from base class.
   const CABFHeader &operator=(const ABFFileHeader &FH);

   // Copy constructor from base class.
   CABFHeader(const ABFFileHeader &FH);
   
   // Destructor. Cleans up when object is destroyed.
   ~CABFHeader();

   // Comparison.
   BOOL operator==(CABFHeader &FH);

   // Helper functions
   void Initialize();
   void SetDateTime(UINT uStopWatch);
   void GetADCDisplayRange(UINT uChannel, float *pfUUTop, float *pfUUBottom) const;
   void GetADCFullScaleRange(UINT uChannel, float *pfUUMax, float *pfUUMin) const;
   
   void GetADCtoUUFactors(UINT uChannel, float *pfADCToUUFactor, float *pfADCToUUShift) const;
   void GetVoltstoUUFactors(UINT uChannel, float *pfVoltToUUFactor, float *pfVoltToUUShift) const;
   
   short ClipADCValue(long lADCValue, BOOL &bClipped) const;
   float ClipADCUUValue(UINT uChannel, float fValue) const;
   short UUtoADC(UINT uChannel, float fUUValue, BOOL &bClipped) const;
   float ADCtoUU(UINT uChannel, short nADCValue) const;
   
   short VoltstoADC(UINT uChannel, float fVolts, BOOL &bClipped) const;
   float ADCtoVolts(UINT uChannel, short nADCValue) const;
   
   float UUtoVolts(UINT uChannel, float fUUValue) const;
   float VoltstoUU(UINT uChannel, float fVValue) const;
   
   void GetDACtoUUFactors(UINT uChannel, float *pfDACToUUFactor, float *pfDACToUUShift) const;
   short ClipDACValue(long lDACValue, BOOL &bClipped) const;
   float ClipDACUUValue(UINT uChannel, float fValue) const;
   short UUtoDAC(UINT uChannel, float fUUValue, BOOL &bClipped) const;
   float DACtoUU(UINT uChannel, short fDACValue) const;
   
   float SamplesToMS(int nSamples) const;

   int  MultiplexOffset(UINT uChannel) const;
   int  MultiplexOffset(LPCSTR pszSignal) const;

   BOOL CheckTriggerChannel() const;
   BOOL SetThreshold(float fThreshold);
   BOOL ChangeThreshold(float fMultiplier);
   BOOL SetDACHolding(UINT uDACNum, float fDACHolding);
   
   BOOL IsThresholdTrigger() const;
   UINT GetTriggerChannel() const;
   UINT GetMxTriggerOffset() const;
   BOOL GetADCTriggerPolarity() const;
   
   BOOL CheckChannelNames() const;
   
   void SetADCName(UINT uChannel, LPCSTR ps);
   void GetADCName(UINT uChannel, char *ps, UINT uSize) const;
   
   void SetADCUnits(UINT uChannel, LPCSTR ps);
   void GetADCUnits(UINT uChannel, char *ps, UINT uSize) const;
   
   void SetMathUnits(LPCSTR ps);
   void GetMathUnits(char *ps, UINT uSize) const;
   void GetMathName(char *ps, UINT uSize) const;

   void SetProtocolPath( LPCSTR ps );

   void SetDACName(UINT uChannel, LPCSTR ps);
   void GetDACName(UINT uChannel, char *ps, UINT uSize) const;
   
   void SetDACUnits(UINT uChannel, LPCSTR ps);
   void GetDACUnits(UINT uChannel, char *ps, UINT uSize) const;
   
   void SetCreatorInfo(LPCSTR psz);
   void GetCreatorInfo(char *psz, UINT uSize) const;
   
   void SetComment(LPCSTR ps);
   void GetComment(char *ps, UINT uSize) const;
   
   void SetUserList( UINT uListNum, LPCSTR ps );
   void GetUserList( UINT uListNum, char *ps, UINT uSize ) const;

   void SetDACFilePath(UINT uChannel, LPCSTR ps);   
   void GetDACFilePath(UINT uChannel, char *ps, UINT uSize) const;
   
   BOOL  CheckMathChannels() const;
   BOOL  CheckMathExpression() const;
   float GetMathValue(float fUUValueA, float fUUValueB) const;

   // Clampex specific additions.
   UINT  GetHoldingDuration() const;
   float GetFirstHoldingLevel( UINT uDAC ) const;

   BOOL  CheckSampleIntervals(float fClockResolution) const;
   float GetRawSampleInterval() const;
   UINT  GetRawEpisodeLength() const;
   UINT  GetClockChange() const;
   UINT  GetOutputSequenceLength(BOOL bSeperateDigOut) const;
   UINT  GetNumActiveDACChannels() const;
   UINT  GetDACSamplingSeq(short *pnSamplingSeq, UINT uSeqSize, BOOL bSeperateDigOut) const;

   double GetEpisodeDuration() const;
   double GetPNDuration( UINT uChannel ) const;
   double GetTrainDuration( UINT uChannel ) const;
   double GetMetaEpisodeDuration() const;
   double GetEpisodeStartToStart() const;
   double GetRunDuration() const;
   double GetRunStartToStart() const;

   // Get the total number of raw samples to be acquired in a trial.
   UINT   GetRawTrialSamples() const;

   float GetFastInterval() const;
   void  SetFastInterval(float fFastInterval);

   BOOL  GetEpochLimits(int nADCNum, UINT uDACNum, UINT uEpisode, int nEpoch, 
                        UINT *puStart, UINT *puEnd, int *pnError=NULL) const;

   double SynchCountToMS(UINT uCount) const;
   UINT   MSToSynchCount(double dTimeMS) const;

   // Get / Set auto analysis parameters.
   void  SetAutoAnalysis( int nEnable, LPCSTR pszMacroName );
   int   GetAutoAnalysis( char *pszMacroName, UINT uSize ) const;

};

//==============================================================================================
// FUNCTION:   GetADCtoUUFactors
// PURPOSE:    Calculates the scaling factors used to convert ADC values to UserUnits.
// PARAMETERS:
//    uChannel        - The physical channel number to get the factors for.
//    pfADCToUUFactor - Pointers to return locations for scale and offset.
//    pfADCToUUShift    UserUnits = ADCValue * fADCToUUFactor + fADCToUUShift;
//
inline void CABFHeader::GetADCtoUUFactors(UINT uChannel, float *pfADCToUUFactor, float *pfADCToUUShift) const
{
   // Validate parameters.
   MEMBERASSERT();
   ASSERT(uChannel < ABF_ADCCOUNT);
   WPTRASSERT(pfADCToUUShift);
   WPTRASSERT(pfADCToUUFactor);
   
   ABFH_GetADCtoUUFactors(this, int(uChannel), pfADCToUUFactor, pfADCToUUShift);
}

//==============================================================================================
// FUNCTION:   ClipADCValue
// PURPOSE:    Limits the ADC value to the range of the A/D converter.
//
inline short CABFHeader::ClipADCValue(long lADCValue, BOOL &bClipped) const
{
   MEMBERASSERT();
   bClipped = TRUE;
   if (lADCValue >= lADCResolution)
      return short(lADCResolution-1);
   if (lADCValue <= -lADCResolution)
      return short(-lADCResolution+1);
   bClipped = FALSE;
   return short(lADCValue);
}

//==============================================================================================
// FUNCTION:   ClipADCUUValue
// PURPOSE:    Limits the UU value to the range of the A/D converter.
//
inline float CABFHeader::ClipADCUUValue(UINT uChannel, float fValue) const
{
   // Validate parameters.
   MEMBERASSERT();
   ASSERT(uChannel < ABF_ADCCOUNT);
   
   ABFH_ClipADCUUValue(this, int(uChannel), &fValue);
   return fValue;
}

//==============================================================================================
// FUNCTION:   UUtoADC
// PURPOSE:    Converts a floating point UU value to a two byte ADC value.
//
inline short CABFHeader::UUtoADC(UINT uChannel, float fUUValue, BOOL &bClipped) const
{
   MEMBERASSERT();
   float fADCToUUFactor, fADCToUUShift;
   GetADCtoUUFactors(uChannel, &fADCToUUFactor, &fADCToUUShift);
   return ClipADCValue(long((fUUValue-fADCToUUShift)/fADCToUUFactor+0.5F), bClipped);
}

//==============================================================================================
// FUNCTION:   ADCtoUU
// PURPOSE:    Converts a two byte ADC value to a floating point UU value.
//
inline float CABFHeader::ADCtoUU(UINT uChannel, short nADCValue) const
{
   MEMBERASSERT();
   float fADCToUUFactor, fADCToUUShift;
   GetADCtoUUFactors(uChannel, &fADCToUUFactor, &fADCToUUShift);
   return float(nADCValue*fADCToUUFactor+fADCToUUShift);
}


//==============================================================================================
// FUNCTION:   UUtoVolts
// PURPOSE:    Converts a floating point UU value to Volts at the ADC.
//
inline float CABFHeader::UUtoVolts(UINT uChannel, float fUUValue) const
{
   MEMBERASSERT();
   float fVoltToUUFactor, fVoltToUUShift;
   GetVoltstoUUFactors(uChannel, &fVoltToUUFactor, &fVoltToUUShift);
   return (fUUValue - fVoltToUUShift) / fVoltToUUFactor;
//   return (fUUValue - fInstrumentOffset[uChannel]) * fInstrumentScaleFactor[uChannel];
}


//==============================================================================================
// FUNCTION:   VoltstoUU
// PURPOSE:    Converts a floating point Voltage (at ADC) to user units.
//
inline float CABFHeader::VoltstoUU(UINT uChannel, float fVValue) const
{
   MEMBERASSERT();
   float fVoltToUUFactor, fVoltToUUShift;
   GetVoltstoUUFactors(uChannel, &fVoltToUUFactor, &fVoltToUUShift);
   return fVValue * fVoltToUUFactor + fVoltToUUShift;
//   return (fVValue / fInstrumentScaleFactor[uChannel]) + fInstrumentOffset[uChannel];
}


//==============================================================================================
// FUNCTION:   VoltstoADC
// PURPOSE:    Converts a value in Volts to an ADC value.
//
inline short CABFHeader::VoltstoADC(UINT uChannel, float fVolts, BOOL &bClipped) const
{
   MEMBERASSERT();
   long lADCValue = long(fVolts/fADCRange*lADCResolution*fADCProgrammableGain[uChannel]+0.5F);
   return ClipADCValue(lADCValue, bClipped);
}


//==============================================================================================
// FUNCTION:   UUtoVolts
// PURPOSE:    Converts an ADC value to Volts at the ADC.
//
inline float CABFHeader::ADCtoVolts(UINT uChannel, short nADCValue) const
{
   MEMBERASSERT();
   return float(nADCValue/fADCProgrammableGain[uChannel])*fADCRange/lADCResolution;
}

   
//==============================================================================================
// FUNCTION:   GetDACtoUUFactors
// PURPOSE:    Calculates the scaling factors used to convert DAC values to UserUnits.
// PARAMETERS:
//    uChannel        - The physical channel number to get the factors for.
//    pfDACToUUFactor - Pointers to return locations for scale and offset.
//    pfDACToUUShift    UserUnits = DACValue * fDACToUUFactor + fDACToUUShift;
//
inline void CABFHeader::GetDACtoUUFactors(UINT uChannel, float *pfDACToUUFactor, float *pfDACToUUShift) const
{
   // Validate parameters.
   MEMBERASSERT();
   ASSERT(uChannel < ABF_DACCOUNT);
   WPTRASSERT(pfDACToUUShift);
   WPTRASSERT(pfDACToUUFactor);
   
   ABFH_GetDACtoUUFactors(this, int(uChannel), pfDACToUUFactor, pfDACToUUShift);
}

//==============================================================================================
// FUNCTION:   ClipDACValue
// PURPOSE:    Limits the DAC value to the range of the A/D converter.
//
inline short CABFHeader::ClipDACValue(long lDACValue, BOOL &bClipped) const
{
   MEMBERASSERT();
   bClipped = TRUE;
   if (lDACValue >= lDACResolution)
      return short(lDACResolution-1);
   if (lDACValue <= -lDACResolution)
      return short(-lDACResolution+1);
   bClipped = FALSE;
   return short(lDACValue);
}

//==============================================================================================
// FUNCTION:   ClipDACUUValue
// PURPOSE:    Limits the UU value to the range of the D/A converter.
//
inline float CABFHeader::ClipDACUUValue(UINT uChannel, float fValue) const
{
   // Validate parameters.
   MEMBERASSERT();
   ASSERT(uChannel < ABF_DACCOUNT);
   
   ABFH_ClipDACUUValue(this, int(uChannel), &fValue);
   return fValue;
}

//==============================================================================================
// FUNCTION:   UUtoDAC
// PURPOSE:    Converts a floating point UU value to a two byte DAC value.
//
inline short CABFHeader::UUtoDAC(UINT uChannel, float fUUValue, BOOL &bClipped) const
{
   MEMBERASSERT();
   float fDACToUUFactor, fDACToUUShift;
   GetDACtoUUFactors(uChannel, &fDACToUUFactor, &fDACToUUShift);
   return ClipDACValue(long((fUUValue-fDACToUUShift)/fDACToUUFactor+0.5F), bClipped);
}

//==============================================================================================
// FUNCTION:   DACtoUU
// PURPOSE:    Converts a two byte DAC value to a floating point UU value.
//
inline float CABFHeader::DACtoUU(UINT uChannel, short nDACValue) const
{
   MEMBERASSERT();
   float fDACToUUFactor, fDACToUUShift;
   GetDACtoUUFactors(uChannel, &fDACToUUFactor, &fDACToUUShift);
   return float(nDACValue*fDACToUUFactor+fDACToUUShift);
}

//==============================================================================================
// FUNCTION:   IsThresholdTrigger
// PURPOSE:    Checks whether the trigger type is data-threshold crossing.
//
inline BOOL CABFHeader::IsThresholdTrigger() const
{
   MEMBERASSERT();
   return (nTriggerSource >= 0) || (nTriggerSource==ABF_TRIGGERFIRSTCHANNEL);
}

//==============================================================================================
// FUNCTION:   GetTriggerChannel
// PURPOSE:    Gets the channel number of the data stream to be used for threshold crossing detection.
//
inline UINT CABFHeader::GetTriggerChannel() const
{
   MEMBERASSERT();
   if (nTriggerSource >= 0)
      return UINT(nTriggerSource);
   ASSERT(nTriggerSource==ABF_TRIGGERFIRSTCHANNEL);
   return UINT(nADCSamplingSeq[0]);
}

//==============================================================================================
// FUNCTION: GetMathValue
// PURPOSE:  Combines the two channel values according to the math channel parameters.
//
inline float CABFHeader::GetMathValue(float fUUValueA, float fUUValueB) const
{
   MEMBERASSERT();
   float fRval = 0.0F;
   ABFH_GetMathValue(this, fUUValueA, fUUValueB, &fRval);
   return fRval;
}

//==============================================================================================
// FUNCTION: GetEpochLimits
// PURPOSE:  Gets the mx samples that bound a given epoch. 
//           ABFH_FIRSTHOLDING and ABFH_LASTHOLDING are both supported.
//
inline BOOL CABFHeader::GetEpochLimits(int nADCNum, UINT uDACNum, UINT uEpisode, int nEpoch, 
                                       UINT *puStart, UINT *puEnd, int *pnError) const
{
   MEMBERASSERT();
   return ABFH_GetEpochLimitsEx(this, nADCNum, uDACNum, uEpisode, nEpoch, puStart, puEnd, pnError);
}

//===============================================================================================
// FUNCTION: SynchTimeToMS
// PURPOSE:  Converts the count in a synch entry to time in milli-seconds.
//
inline double CABFHeader::SynchCountToMS(UINT uCount) const
{
   MEMBERASSERT();
   double dTimeMS = 0.0;
   ABFH_SynchCountToMS(this, uCount, &dTimeMS);
   return dTimeMS;
}

//===============================================================================================
// FUNCTION: MSToSynchCount
// PURPOSE:  Converts a time value to a synch time count.
//
inline UINT CABFHeader::MSToSynchCount(double dTimeMS) const
{
   MEMBERASSERT();
   return ABFH_MSToSynchCount(this, dTimeMS);
}

#endif   // INC_CABFHDR_HPP
