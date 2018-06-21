//***********************************************************************************************
//
//    Copyright (c) 1993-1997 Axon Instruments.
//    All rights reserved.
//    Permission is granted to freely to use, modify and copy the code in this file.
//
//***********************************************************************************************
// This is CPARAM.C; A module of HEADER.EXE for setting/showing specific
//                   parameters from an ABF file header.
// 
// An ANSI C compiler should be used for compilation.
// Compile with the large memory model option.
// (e.g. CL -c -AL ATFTST.C ATFFILES.C)

#include "wincpp.hpp"

#include "cparam.h"
#include "abffiles.h"
#include "abfheadr.h"

static char *CP_Errors[] = 
{   
   "",
   "Cannot open file",
   "Cannot read from file",
   "Cannot seek",
   "Cannot write to file",
   "Parameter not found",
   "Invalid file. Only ABF format supported",
   "Index out of range"
};

#define  PARAM_STRING    1
#define  PARAM_SHORT     2
#define  PARAM_LONG      3
#define  PARAM_FLOAT     4
#define  PARAM_BYTE      5

#define  ABFNAME(i)        (ABFParameters[i].Name)
#define  ABFTYPE(i)        (ABFParameters[i].Type)
#define  ABFNUMARGS(i)     (ABFParameters[i].NumArgs)
#define  ABFSIZE(i)        (ABFParameters[i].Size)
#define  ABFADDRESS(i)     (ABFParameters[i].Address)
#define  ABFSVALUE(i)      (((char*)ABFParameters[i].Address+ABFSIZE(i)*CPIndex))
#define  ABFNVALUE(i)      (*((short*)ABFParameters[i].Address+CPIndex))
#define  ABFLVALUE(i)      (*((long*)ABFParameters[i].Address+CPIndex))
#define  ABFFVALUE(i)      (*((float*)ABFParameters[i].Address+CPIndex))

static void SetParam(int i, int CPIndex, char *pszVal);
static void ShowParam(int i, int CPIndex, int bNewValue,
                      int bShowIndexAsChar);

typedef struct
{   
   char     *Name;         // eg. lActualEpisodes
   void     *Address;      // eg. &pFH->lActualEpisodes
   int      Type;          // eg. PARAM_LONG
   int      NumArgs;       // eg. DAC_COUNT
   int      Size;          // eg. FILETYPELEN
} ABFParameter;

static ABFFileHeader ABFFH;

static ABFParameter ABFParameters[] =
{   
   // GROUP #1 (40 bytes)

   {"lFileSignature",            &ABFFH.lFileSignature,
                                 PARAM_LONG, 1, sizeof(long)},

   {"fFileVersionNumber",        &ABFFH.fFileVersionNumber,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"nOperationMode",            &ABFFH.nOperationMode,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"lActualAcqLength",          &ABFFH.lActualAcqLength,
                                 PARAM_LONG, 1, sizeof(long)},

   {"nNumPointsIgnored",         &ABFFH.nNumPointsIgnored,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"lActualEpisodes",           &ABFFH.lActualEpisodes,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lFileStartDate",            &ABFFH.lFileStartDate,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lFileStartTime",            &ABFFH.lFileStartTime,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lStopwatchTime",            &ABFFH.lStopwatchTime,
                                 PARAM_LONG, 1, sizeof(long)},

   {"fHeaderVersionNumber",      &ABFFH.fHeaderVersionNumber,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"nFileType",                 &ABFFH.nFileType,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nMSBinFormat",              &ABFFH.nMSBinFormat,
                                 PARAM_SHORT, 1, sizeof(short)},

   // GROUP #2 (80 bytes)

   {"lDataSectionPtr",           &ABFFH.lDataSectionPtr,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lTagSectionPtr",            &ABFFH.lTagSectionPtr,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lNumTagEntries",            &ABFFH.lNumTagEntries,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lScopeConfigPtr",           &ABFFH.lScopeConfigPtr,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lNumScopes",                &ABFFH.lNumScopes,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lDACFilePtr",               &ABFFH.lDACFilePtr,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lDACFileNumEpisodes",       &ABFFH.lDACFileNumEpisodes,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lDeltaArrayPtr",            &ABFFH.lDeltaArrayPtr,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lNumDeltas",                &ABFFH.lNumDeltas,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lSynchArrayPtr",            &ABFFH.lSynchArrayPtr,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lSynchArraySize",           &ABFFH.lSynchArraySize,
                                 PARAM_LONG, 1, sizeof(long)},

   {"nDataFormat",               &ABFFH.nDataFormat,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nSimultaneousScan",         &ABFFH.nSimultaneousScan,
                                 PARAM_SHORT, 1, sizeof(short)},

   // GROUP #3 (80 bytes)

   {"nADCNumChannels",           &ABFFH.nADCNumChannels,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"fADCSampleInterval",        &ABFFH.fADCSampleInterval,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fADCSecondSampleInterval",  &ABFFH.fADCSecondSampleInterval,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fSynchTimeUnit",            &ABFFH.fSynchTimeUnit,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fSecondsPerRun",            &ABFFH.fSecondsPerRun,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"lNumSamplesPerEpisode",     &ABFFH.lNumSamplesPerEpisode,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lPreTriggerSamples",        &ABFFH.lPreTriggerSamples,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lEpisodesPerRun",           &ABFFH.lEpisodesPerRun,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lRunsPerTrial",             &ABFFH.lRunsPerTrial,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lNumberOfTrials",           &ABFFH.lNumberOfTrials,
                                 PARAM_LONG, 1, sizeof(long)},

   {"nAveragingMode",            &ABFFH.nAveragingMode,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nUndoRunCount",             &ABFFH.nUndoRunCount,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nFirstEpisodeInRun",        &ABFFH.nFirstEpisodeInRun,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"fTriggerThreshold",         &ABFFH.fTriggerThreshold,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"nTriggerSource",            &ABFFH.nTriggerSource,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nTriggerAction",            &ABFFH.nTriggerAction,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nTriggerPolarity",          &ABFFH.nTriggerPolarity,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"fScopeOutputInterval",      &ABFFH.fScopeOutputInterval,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fEpisodeStartToStart",      &ABFFH.fEpisodeStartToStart,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fRunStartToStart",          &ABFFH.fRunStartToStart,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fTrialStartToStart",        &ABFFH.fTrialStartToStart,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"lAverageCount",             &ABFFH.lAverageCount,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lClockChange",              &ABFFH.lClockChange,
                                 PARAM_LONG, 1, sizeof(long)},

   // GROUP #4 (44 bytes)

   {"nDrawingStrategy",          &ABFFH.nDrawingStrategy,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nTiledDisplay",             &ABFFH.nTiledDisplay,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nEraseStrategy",            &ABFFH.nEraseStrategy,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nDataDisplayMode",          &ABFFH.nDataDisplayMode,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"lDisplayAverageUpdate",     &ABFFH.lDisplayAverageUpdate,
                                 PARAM_LONG, 1, sizeof(long)},

   {"nChannelStatsStrategy",     &ABFFH.nChannelStatsStrategy,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"lCalculationPeriod",        &ABFFH.lCalculationPeriod,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lSamplesPerTrace",          &ABFFH.lSamplesPerTrace,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lStartDisplayNum",          &ABFFH.lStartDisplayNum,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lFinishDisplayNum",         &ABFFH.lFinishDisplayNum,
                                 PARAM_LONG, 1, sizeof(long)},

   {"nMultiColor",               &ABFFH.nMultiColor,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nShowPNRawData",            &ABFFH.nShowPNRawData,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"fStatisticsPeriod",         &ABFFH.fStatisticsPeriod,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"lStatisticsMeasurements",   &ABFFH.lStatisticsMeasurements,
                                 PARAM_LONG, 1, sizeof(long)},

   {"nStatisticsSaveStrategy",   &ABFFH.nStatisticsSaveStrategy,
                                 PARAM_SHORT, 1, sizeof(short)},

   // GROUP #5 (16 bytes)

   {"fADCRange",                 &ABFFH.fADCRange,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fDACRange",                 &ABFFH.fDACRange,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"lADCResolution",            &ABFFH.lADCResolution,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lDACResolution",            &ABFFH.lDACResolution,
                                 PARAM_LONG, 1, sizeof(long)},

   // GROUP #6 (118 bytes)

   {"nExperimentType",           &ABFFH.nExperimentType,
                                 PARAM_SHORT, 1, sizeof(short)},

// FIX FIX FIX PRC Telegraph changes - check !
   {"nAutosampleEnable",         &ABFFH._nAutosampleEnable,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nAutosampleADCNum",         &ABFFH._nAutosampleADCNum,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nAutosampleInstrument",     &ABFFH._nAutosampleInstrument,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"fAutosampleAdditGain",      &ABFFH._fAutosampleAdditGain,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fAutosampleFilter",         &ABFFH._fAutosampleFilter,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fAutosampleMembraneCap",    &ABFFH._fAutosampleMembraneCap,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"nManualInfoStrategy",       &ABFFH.nManualInfoStrategy,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nCommentsEnable",           &ABFFH.nCommentsEnable,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"fCellID1",                  &ABFFH.fCellID1,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fCellID2",                  &ABFFH.fCellID2,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fCellID3",                  &ABFFH.fCellID3,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"sCreatorInfo",              &ABFFH.sCreatorInfo[0],
                                 PARAM_STRING, 1, ABF_CREATORINFOLEN},

   {"sFileComment",              &ABFFH.sFileComment[0],
                                 PARAM_STRING, 1, ABF_FILECOMMENTLEN},

   // GROUP #7 (1044 (160 + 384 + 488 + 12) bytes) - Multi-channel information.

   {"nADCPtoLChannelMap",        &ABFFH.nADCPtoLChannelMap[0],
                                 PARAM_SHORT, ABF_ADCCOUNT, sizeof(short)},

   {"nADCSamplingSeq",           &ABFFH.nADCSamplingSeq[0],
                                 PARAM_SHORT, ABF_ADCCOUNT, sizeof(short)},

   {"sADCChannelName",           &ABFFH.sADCChannelName[0],
                                 PARAM_STRING, ABF_ADCCOUNT, ABF_ADCNAMELEN},

   {"sADCUnits",                 &ABFFH.sADCUnits[0],
                                 PARAM_STRING, ABF_ADCCOUNT, ABF_ADCUNITLEN},

   {"fADCProgrammableGain",      &ABFFH.fADCProgrammableGain[0],
                                 PARAM_FLOAT, ABF_ADCCOUNT, sizeof(float)},

   {"fADCDisplayAmplification",  &ABFFH.fADCDisplayAmplification[0],
                                 PARAM_FLOAT, ABF_ADCCOUNT, sizeof(float)},

   {"fADCDisplayOffset",         &ABFFH.fADCDisplayOffset[0],
                                 PARAM_FLOAT, ABF_ADCCOUNT, sizeof(float)},

   {"fInstrumentScaleFactor",    &ABFFH.fInstrumentScaleFactor[0],
                                 PARAM_FLOAT, ABF_ADCCOUNT, sizeof(float)},

   {"fInstrumentOffset",         &ABFFH.fInstrumentOffset[0],
                                 PARAM_FLOAT, ABF_ADCCOUNT, sizeof(float)},

   {"fSignalGain",               &ABFFH.fSignalGain[0],
                                 PARAM_FLOAT, ABF_ADCCOUNT, sizeof(float)},

   {"fSignalOffset",             &ABFFH.fSignalOffset[0],
                                 PARAM_FLOAT, ABF_ADCCOUNT, sizeof(float)},

   {"fSignalLowpassFilter",      &ABFFH.fSignalLowpassFilter[0],
                                 PARAM_FLOAT, ABF_ADCCOUNT, sizeof(float)},

   {"fSignalHighpassFilter",     &ABFFH.fSignalHighpassFilter[0],
                                 PARAM_FLOAT, ABF_ADCCOUNT, sizeof(float)},

   {"sDACChannelName",           &ABFFH.sDACChannelName[0],
                                 PARAM_STRING, ABF_DACCOUNT, ABF_DACNAMELEN},

   {"sDACChannelUnits",          &ABFFH.sDACChannelUnits[0],
                                 PARAM_STRING, ABF_DACCOUNT, ABF_DACUNITLEN},

   {"fDACScaleFactor",           &ABFFH.fDACScaleFactor[0],
                                 PARAM_FLOAT, ABF_DACCOUNT, sizeof(float)},

   {"fDACHoldingLevel",          &ABFFH.fDACHoldingLevel[0],
                                 PARAM_FLOAT, ABF_DACCOUNT, sizeof(float)},

   {"nSignalType",               &ABFFH.nSignalType,
                                 PARAM_SHORT, 1, sizeof(short)},

   // GROUP #8 (14 bytes) - Synchronous timer outputs.

   {"nOUTEnable",                &ABFFH.nOUTEnable,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nSampleNumberOUT1",         &ABFFH.nSampleNumberOUT1,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nSampleNumberOUT2",         &ABFFH.nSampleNumberOUT2,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nFirstEpisodeOUT",          &ABFFH.nFirstEpisodeOUT,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nLastEpisodeOUT",           &ABFFH.nLastEpisodeOUT,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nPulseSamplesOUT1",         &ABFFH.nPulseSamplesOUT1,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nPulseSamplesOUT2",         &ABFFH.nPulseSamplesOUT2,
                                 PARAM_SHORT, 1, sizeof(short)},

   // GROUP #9 (184 bytes) - Epoch Output Waveform and Pulses;

   {"nDigitalEnable",            &ABFFH.nDigitalEnable,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nWaveformEnable",           &ABFFH.nWaveformEnable,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nWaveformSource",           &ABFFH.nWaveformSource,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nActiveDACChannel",         &ABFFH.nActiveDACChannel,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nInterEpisodeLevel",        &ABFFH.nInterEpisodeLevel,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nEpochType",                &ABFFH.nEpochType[0],
                                 PARAM_SHORT, ABF_EPOCHCOUNT, sizeof(short)},

   {"fEpochInitLevel",           &ABFFH.fEpochInitLevel[0],
                                 PARAM_FLOAT, ABF_EPOCHCOUNT, sizeof(float)},

   {"fEpochLevelInc",            &ABFFH.fEpochLevelInc[0],
                                 PARAM_FLOAT, ABF_EPOCHCOUNT, sizeof(float)},

   {"lEpochInitDuration",        &ABFFH.lEpochInitDuration[ABFFH.nActiveDACChannel][0],
                                 PARAM_SHORT, ABF_EPOCHCOUNT, sizeof(long)},

   {"lEpochDurationInc",         &ABFFH.lEpochDurationInc[ABFFH.nActiveDACChannel][0],
                                 PARAM_SHORT, ABF_EPOCHCOUNT, sizeof(long)},

   {"nDigitalHolding",           &ABFFH.nDigitalHolding,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nDigitalInterEpisode",      &ABFFH.nDigitalInterEpisode,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nDigitalValue",             &ABFFH.nDigitalValue[0],
                                 PARAM_SHORT, ABF_EPOCHCOUNT, sizeof(short)},

   // GROUP #10 (98 bytes) - Analog Output File Waveform

   {"fDACFileScale",             &ABFFH.fDACFileScale,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fDACFileOffset",            &ABFFH.fDACFileOffset,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"lDACFileEpisodeNum",        &ABFFH.lDACFileEpisodeNum,
                                 PARAM_SHORT, 1, sizeof(long)},

   {"nDACFileADCNum",            &ABFFH.nDACFileADCNum,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"sDACFilePath",              &ABFFH.sDACFilePath[0],
                                 PARAM_STRING, 1, ABF_PATHLEN},

   // GROUP #11 (44 bytes) - Conditioning pulse train.

   {"nConditEnable",             &ABFFH.nConditEnable[0],
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nConditChannel",            &ABFFH._nConditChannel,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"lConditNumPulses",          &ABFFH.lConditNumPulses[0],
                                 PARAM_LONG, 1, sizeof(long)},

   {"fBaselineDuration",         &ABFFH.fBaselineDuration[0],
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fBaselineLevel",            &ABFFH.fBaselineLevel[0],
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fStepDuration",             &ABFFH.fStepDuration[0],
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fStepLevel",                &ABFFH.fStepLevel[0],
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fPostTrainPeriod",          &ABFFH.fPostTrainPeriod[0],
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fPostTrainLevel",           &ABFFH.fPostTrainLevel[0],
                                 PARAM_FLOAT, 1, sizeof(float)},

   // GROUP #12 ( 82 bytes) - Variable parameter list.

   {"nParamToVary",              &ABFFH.nULParamToVary[0],
                                 PARAM_SHORT, 1, sizeof(short)},

   {"sParamValueList",           &ABFFH.sULParamValueList[0],
                                 PARAM_STRING, 1, ABF_USERLISTLEN},

   // GROUP #13 (36 bytes) - Autopeak measurement.

   {"nAutopeakEnable",           &ABFFH.nAutopeakEnable,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nAutopeakPolarity",         &ABFFH.nAutopeakPolarity,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nAutopeakADCNum",           &ABFFH.nAutopeakADCNum,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nAutopeakSearchMode",       &ABFFH.nAutopeakSearchMode,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"lAutopeakStart",            &ABFFH.lAutopeakStart,
                                 PARAM_LONG, 1, sizeof(long)},

   {"lAutopeakEnd",              &ABFFH.lAutopeakEnd,
                                 PARAM_LONG, 1, sizeof(long)},

   {"nAutopeakSmoothing",        &ABFFH.nAutopeakSmoothing,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nAutopeakBaseline",         &ABFFH.nAutopeakBaseline,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nAutopeakAverage",          &ABFFH.nAutopeakAverage,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"lAutopeakMeasurements",     &ABFFH.lAutopeakMeasurements,
                                 PARAM_LONG, 1, sizeof(long)},

   // GROUP #14 (52 bytes) - Channel Arithmetic

   {"nArithmeticEnable",         &ABFFH.nArithmeticEnable,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"fArithmeticUpperLimit",     &ABFFH.fArithmeticUpperLimit,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fArithmeticLowerLimit",     &ABFFH.fArithmeticLowerLimit,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"nArithmeticADCNumA",        &ABFFH.nArithmeticADCNumA,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nArithmeticADCNumB",        &ABFFH.nArithmeticADCNumB,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"fArithmeticK1",             &ABFFH.fArithmeticK1,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fArithmeticK2",             &ABFFH.fArithmeticK2,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fArithmeticK3",             &ABFFH.fArithmeticK3,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fArithmeticK4",             &ABFFH.fArithmeticK4,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"sArithmeticOperator",       &ABFFH.sArithmeticOperator[0],
                                 PARAM_STRING, 1, ABF_ARITHMETICOPLEN},

   {"sArithmeticUnits",          &ABFFH.sArithmeticUnits[0],
                                 PARAM_STRING, 1, ABF_ARITHMETICUNITSLEN},

   {"fArithmeticK5",             &ABFFH.fArithmeticK5,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fArithmeticK6",             &ABFFH.fArithmeticK6,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"nArithmeticExpression",     &ABFFH.nArithmeticExpression,
                                 PARAM_SHORT, 1, sizeof(short)},

   // GROUP #15 (34 bytes) - On-line subtraction.

   {"nPNEnable",                 &ABFFH.nPNEnable,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nPNPosition",               &ABFFH.nPNPosition,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nPNPolarity",               &ABFFH.nPNPolarity,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nPNNumPulses",              &ABFFH.nPNNumPulses,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nPNADCNum",                 &ABFFH.nPNADCNum,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"fPNHoldingLevel",           &ABFFH.fPNHoldingLevel,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fPNSettlingTime",           &ABFFH.fPNSettlingTime,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"fPNInterpulse",             &ABFFH.fPNInterpulse,
                                 PARAM_FLOAT, 1, sizeof(float)},

   // GROUP #16 (54 bytes) - Unused space at end of header block.

   {"nListEnable",               &ABFFH.nULEnable[0],
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nBellEnable",               &ABFFH.nBellEnable[0],
                                 PARAM_SHORT, ABF_BELLCOUNT, sizeof(short)},

   {"nBellLocation",             &ABFFH.nBellLocation[0],
                                 PARAM_SHORT, ABF_BELLCOUNT, sizeof(short)},

   {"nBellRepetitions",          &ABFFH.nBellRepetitions[0],
                                 PARAM_SHORT, ABF_BELLCOUNT, sizeof(short)},

   {"nLevelHysteresis",          &ABFFH.nLevelHysteresis,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"lTimeHysteresis",           &ABFFH.lTimeHysteresis,
                                 PARAM_LONG, 1, sizeof(long)},

   {"nAllowExternalTags",        &ABFFH.nAllowExternalTags,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nLowpassFilterType",        &ABFFH.nLowpassFilterType[0],
                                 PARAM_BYTE, ABF_ADCCOUNT, sizeof(char)},

   {"nHighpassFilterType",       &ABFFH.nHighpassFilterType[0],
                                 PARAM_BYTE, ABF_ADCCOUNT, sizeof(char)},

   {"nAverageAlgorithm",         &ABFFH.nAverageAlgorithm,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"fAverageWeighting",         &ABFFH.fAverageWeighting,
                                 PARAM_FLOAT, 1, sizeof(float)},

   {"nUndoPromptStrategy",       &ABFFH.nUndoPromptStrategy,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nTrialTriggerSource",       &ABFFH.nTrialTriggerSource,
                                 PARAM_SHORT, 1, sizeof(short)},

   {"nStatisticsDisplayStrategy", &ABFFH.nStatisticsDisplayStrategy,
                                  PARAM_SHORT, 1, sizeof(short)},

   {"nStatisticsClearStrategy",   &ABFFH.nStatisticsClearStrategy,
                                  PARAM_SHORT, 1, sizeof(short)},


   {"",                          NULL,
                                 0, 0, 0},
};

//==============================================================================================
// FUNCTION:   CP_SetShowParam
// PURPOSE:    
// PARAMETERS: Set Parameter and then Show it
//    pszFileName - file name
//    pszParam    - string containing parameter value
//    pszVal
//    CPIndex
//    bShowIndexAsChar
// RETURNS:
//    current argument string
//
int CP_SetShowParam(char *pszFileName, char *pszParam, char *pszVal, int CPIndex, int bShowIndexAsChar)
{   
   int i, nError;
   HANDLE hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

   if (hFile == INVALID_HANDLE_VALUE)
      return CP_EOPENFILE;

   BOOL bOK = ABFH_ParamReader(hFile, &ABFFH, &nError);
   CloseHandle(hFile);

   if (!bOK)
      return(CP_EINVALIDFILE);

   for (i = 0; ABFTYPE(i); i++)
   {   
      if (stricmp(pszParam, ABFNAME(i)) == 0)  // found it
         break;
   }

   if (!ABFTYPE(i))
      return(CP_ENOTFOUND);

   if ((CPIndex >= ABFNUMARGS(i)) || (CPIndex < 0))
      return(CP_EBADINDEX);

   if (pszVal)
   {   
      SetParam(i, CPIndex, pszVal);

      hFile = CreateFile(pszFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                         OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
      if (hFile == INVALID_HANDLE_VALUE)
         return CP_EOPENFILE;

      // Write the header out to the file.
      DWORD dwBytesWritten;
      bOK = WriteFile(hFile, &ABFFH, ABF_HEADERSIZE, &dwBytesWritten, NULL);
      CloseHandle(hFile);

      if (!bOK)
         return CP_EINVALIDFILE;
   }

   ShowParam(i, CPIndex, pszVal!=NULL, bShowIndexAsChar);

   return (CP_SUCCESS);
}

//==============================================================================================
// FUNCTION:   SetParam
// PURPOSE:   Set a parameter value
// PARAMETERS:
//    i        - parameter for ABFTYPE fn, to specify data type of parameter
//    CPIndex  - parameter index
//    pszVal   - string containing parameter value
//
static void SetParam(int i, int CPIndex, char *pszVal)
{   
   char szBuf[256];

   switch(ABFTYPE(i))
   {   
      case PARAM_STRING:
         memset(szBuf, ' ', ABFSIZE(i));
         szBuf[ABFSIZE(i)] = '\0';
         memcpy(szBuf, pszVal, strlen(pszVal));
         memcpy(ABFSVALUE(i), szBuf, ABFSIZE(i) );
         break;
      case PARAM_SHORT:
         ABFNVALUE(i) = (short)strtol(pszVal, &pszVal, 10);
         break;
      case PARAM_BYTE:
         ABFNVALUE(i) = (char)strtol(pszVal, &pszVal, 10);
         break;
      case PARAM_LONG:
         ABFLVALUE(i) = strtol(pszVal, &pszVal, 10);
         break;
      case PARAM_FLOAT:
         ABFFVALUE(i) = (float)strtod(pszVal, &pszVal);
         break;
   }
}

//==============================================================================================
// FUNCTION:   ShowParam
// PURPOSE:    Display the value of the requested parameter
// PARAMETERS:
//    i                - parameter for ABFTYPE fn, to specify data type of parameter
//    CPIndex          - parameter index
//    bNewValue        - flag to identify parameter as a new value
//    bShowIndexAsChar - flag to indicate whether to display index as char or numeric value
//
static void ShowParam(int i, int CPIndex, int bNewValue, int bShowIndexAsChar)
{   
   char szBuf[256];
   char szIndex[10];
   char *pszNow;

   pszNow = bNewValue ? "now " : "";

   if (ABFNUMARGS(i) < 2)
      szIndex[0] = '\0';
   else if (bShowIndexAsChar)
      sprintf(szIndex, "(%c)", 'A' + CPIndex);
   else
      sprintf(szIndex, "(%d)", CPIndex);

   fprintf(stdout, "\n%s%s %scontains the ", ABFNAME(i), szIndex, pszNow);
   switch(ABFTYPE(i))
   {   
      case PARAM_STRING:
         memcpy(szBuf, ABFSVALUE(i), ABFSIZE(i));
         szBuf[ABFSIZE(i)] = '\0';
         fprintf(stdout, "string ");
         if (ABFSIZE(i) > 40)
            fprintf(stdout, "\n");
         fprintf(stdout, "`%s'.\n", szBuf);
         break;
      case PARAM_SHORT:
         fprintf(stdout, "2-byte short integer %d.\n", ABFNVALUE(i));
         break;
      case PARAM_LONG:
         fprintf(stdout, "4-byte long integer %ld.\n", ABFLVALUE(i));
         break;
      case PARAM_FLOAT:
         fprintf(stdout, "4-byte real %g.\n", ABFFVALUE(i));
         break;
   }
}

//==============================================================================================
// FUNCTION:   CP_ShowError
// PURPOSE:    Display error message
// PARAMETERS:
//    ErrorNum - number of errors
//
void CP_ShowError(int ErrorNum)
{   
   char szBuf[80];

   sprintf(szBuf, "\nError: %s.\n", CP_Errors[ErrorNum]);
   fprintf(stdout, szBuf);
}

