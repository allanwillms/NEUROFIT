/*********************************************************
 *
 *   AbfInter.c - Python interface module to Axon ABF
 *   ==========   file access library for Fulltrace
 *
 *               Greg Ewing, July 2001
 *
 *********************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <wtypes.h>
#include <string.h>

#include "Python.h"
#include "../../AxonDev/Comp/AxAbfFio32/ABFFILES.H"

#define MIN(a,b)  ((a) <= (b) ? (a) : (b))

static int debug = 0;
static PyObject *ABFError;

struct ABFFile {
  int isopen;
  char *name;
  int handle;
  int sweepLength;
  ABFFileHeader header;
};

/*---------------------- Error Handling -----------------------*/

/*
 *  Raise an exception based on an Axon error number.
 */

static void AxonErrorExtended(ABFFile *abf, int err, char *fmt...) {
  va_list ap;
  char buf1[2000], buf2[2000];
  
  va_start(ap, fmt);
  if (!ABF_BuildErrorText(err, abf->name, buf1, sizeof(buf1)))
    sprintf(buf1, "'%s': Unknown ABF error %d", abf->name, err);
  vsprintf(buf2, fmt, ap);
  if (debug)
    printf("ABFInter: ERROR: %s %s\n", buf1, buf2);
  PyErr_Format(ABFError, "%s %s", buf1, buf2);
  va_end(ap);
}

static void AxonError(ABFFile *abf, int err) {
  AxonErrorExtended(abf, err, "");
}

/*---------------------- ABF File Management -----------------------*/

/*
 *   Open abf file. Returns 0 on success, -1 on error.
 */

static int open_abf_file(ABFFile *abf, char *name) {
  UINT maxSamples = 0;
  DWORD maxEpi = 0;
  int err;
  ABFFileHeader *header_old;
  
  abf->name = name;
  if (debug) {
    printf("========================================================\n");
    printf("ABFInter: Opening '%s'\n", abf->name);
  }
  if (!ABF_ReadOpen(abf->name, &abf->handle, ABF_DATAFILE, &abf->header,
        &maxSamples, &maxEpi, &err)) {
    AxonError(abf, err);
    return -1;
  }
  if (!ABFH_IsNewHeader(&abf->header)) {
	  header_old = (ABFFileHeader *) malloc(sizeof(ABFFileHeader));
	  memcpy(header_old, &abf->header, sizeof(ABFFileHeader));
	  ABFH_PromoteHeader(&abf->header, header_old);
	  free(header_old);
  }
  abf->isopen = 1;
  return 0;
}

/*
 *   Close ABF file.
 */

static void close_abf_file(ABFFile *abf) {
  int err;
  if (abf->isopen) {
    ABF_Close(abf->handle, &err);
    abf->isopen = 0;
  }
}

/*---------------------- Utility Functions -----------------------*/

/*
 *   Create a list of time values for the given channel.
 */

static PyObject *make_times(ABFFile *abf, int channel) {
  int i, size;
  float *time = 0;
  PyObject *py_t = 0;
  PyObject *py_times = 0;
  PyObject *py_result = 0;

  if (debug)
    printf("ABFInter: Making times for channel %d\n", channel);
  size = abf->sweepLength;
  time = (float *) malloc(size*sizeof(float));
  ABFH_GetTimebase(&abf->header, 0.0, time, size);
  py_times = PyList_New(size);
  if (!py_times)
    goto done;
  for (i = 0; i < size; i++) {
    py_t = PyFloat_FromDouble((double) time[i]);
    if (!py_t)
      goto done;
    if (PyList_SET_ITEM(py_times, i, py_t) < 0)
      goto done;
  }
  py_t = 0;
  py_result = py_times;
done:
  free(time);
  Py_XDECREF(py_t);
  return py_result;
}

/*
 *   Find the first epoch that has varying output levels.  If the exp_type
 *   is 0 (varying step) then this epoch is the fit epoch, otherwise exp_type
 *   is 1 (varying prestep) and the next epoch is the fit epoch.
 */

static void get_fit_epoch_info(ABFFile *abf, int channel, int sweep,
	int exp_type,
    UINT *fit_epoch_start_sample, UINT *fit_epoch_end_sample,
	float *fit_epoch_volt, float *prev_epoch_volt) {
  int DACchan, i, fit_epoch, err;

  DACchan = abf->header.nActiveDACChannel;
  fit_epoch = -1;
  if (abf->header.nWaveformEnable[DACchan] && 
	  abf->header.nWaveformSource[DACchan] == ABF_EPOCHTABLEWAVEFORM) {
	  i = 0;
	  while (i<ABF_EPOCHCOUNT && fit_epoch == -1) {
		  if (abf->header.nEpochType[DACchan][i] == ABF_EPOCHSTEPPED &&
			  abf->header.fEpochLevelInc[DACchan][i] != 0.0F) 
			  fit_epoch = i + exp_type;
		  else i++;
	  }
  }
  /* I need yet to do some error checking here:
  if (fit_epoch<0 || fit_epoch>=ABF_EPOCHCOUNT) {
      do some error stuff here
  }
  */
  *fit_epoch_volt = abf->header.fEpochInitLevel[DACchan][fit_epoch] +
	  sweep * abf->header.fEpochLevelInc[DACchan][fit_epoch];
  if (fit_epoch == 0) 
	  *prev_epoch_volt = abf->header.fDACHoldingLevel[DACchan];
  else 
	  *prev_epoch_volt = abf->header.fEpochInitLevel[DACchan][fit_epoch-1] +
	      sweep * abf->header.fEpochLevelInc[DACchan][fit_epoch-1];
  if (!ABFH_GetEpochLimits(&abf->header, channel, sweep + 1, fit_epoch,
	  fit_epoch_start_sample, fit_epoch_end_sample, &err)) {
	 AxonErrorExtended(abf, err, 
      "Error occurred while attempting to get epoch limits for sweep %d, channel %d.",
      sweep, channel);
     goto done;
  }
done:
  return;
}

/*
 *   Read samples from the specified channel.
 *   Returns a Python list samples = [sample,...].
 */

static PyObject *read_sweep(ABFFile *abf, int channel, int sweep) {
  int err;
  UINT i, size, size_read;
  float *samples;
  PyObject *py_sample = 0;
  PyObject *py_samples = 0;
  
  size = abf->sweepLength;
  samples = (float *)malloc(size * sizeof(float));
  if (!samples) {
    PyErr_NoMemory();
    goto done;
  }
  if (!ABF_ReadChannel(abf->handle, &abf->header, channel, sweep + 1, 
      samples, &size_read, &err)) {
    AxonErrorExtended(abf, err, 
      "Error occurred while attempting to read sweep %d of channel %d.",
      sweep, channel);
    goto done;
  }
  if (debug)
    printf("......read %d samples\n", size_read);
  if (size_read != size) {
    fprintf(stderr, 
      "ABFInter: Actual size of sweep (%d) does not match expected size (%d)\n", 
      size_read, size);
    exit(1);
  }
  py_samples = PyList_New(size);
  if (!py_samples)
    goto done;
  for (i = 0; i < size; i++) {
    py_sample = PyFloat_FromDouble((double) samples[i]);
    if (PyList_SET_ITEM(py_samples, i, py_sample) < 0)
      goto done;
  }
  py_sample = 0;
done:
  free(samples);
  Py_XDECREF(py_sample);
  return py_samples;
}

/*
 *   Read data from the specified channel. Returns a
 *   Python tuple ([time,...], [sweep,...]) where
 *   sweep = (fit_epoch_start_sample, fit_epoch_end_sample, 
 *            fit_epoch_volt, prev_epoch_volt, samples) where
 *   samples = [sample,...].
 */

static PyObject *read_channel(ABFFile *abf, int channel, int exp_type) {
  PyObject *py_times = 0;
  PyObject *py_sweeps = 0;
  PyObject *py_sweep = 0;
  PyObject *py_samples = 0;
  PyObject *result = 0;
  int sweep, numswps;
  UINT fit_epoch_start_sample, fit_epoch_end_sample;
  float fit_epoch_volt, prev_epoch_volt;
  
  py_times = make_times(abf, channel);
  if (!py_times)
    goto done;

  numswps = MIN(abf->header.lEpisodesPerRun, abf->header.lActualEpisodes);
  py_sweeps = PyList_New(numswps);
  if (!py_sweeps)
    goto done;
  for (sweep = 0; sweep < numswps; sweep++) {
	  get_fit_epoch_info(abf, channel, sweep, exp_type, 
		  &fit_epoch_start_sample, &fit_epoch_end_sample, 
		  &fit_epoch_volt, &prev_epoch_volt);
	  py_samples = read_sweep(abf, channel, sweep);
	  py_sweep = Py_BuildValue("(iiffO)", fit_epoch_start_sample,
		  fit_epoch_end_sample, fit_epoch_volt, prev_epoch_volt,
		  py_samples);
	  if (PyList_SET_ITEM(py_sweeps, sweep, py_sweep) < 0)
		  goto done;
  }
  py_sweep = 0;
  result = Py_BuildValue("(OO)", py_times, py_sweeps);
done:
  Py_XDECREF(py_times);
  Py_XDECREF(py_sweeps);
  Py_XDECREF(py_sweep);
  Py_XDECREF(py_samples);
  return result;
}

/*
 *   get_channel_info -- Get names and units of channels
 */

static PyObject *get_channel_info(ABFFile *abf) {
  int log_chan;
  PyObject *result = 0;
  PyObject *tuple = 0;
  
  result = PyList_New(0);
  if (!result)
    goto bad;
  for (log_chan = 0; log_chan < ABF_ADCCOUNT; log_chan++) {
    int phys_chan = abf->header.nADCSamplingSeq[log_chan];
    if (phys_chan >= 0) {
      char *chan_name = abf->header.sADCChannelName[phys_chan];
      char *units = abf->header.sADCUnits[phys_chan];
      tuple = Py_BuildValue("is#s#", 
        log_chan, 
        chan_name, ABF_ADCNAMELEN,
        units, ABF_ADCUNITLEN);
      if (!tuple)
        goto bad;
      if (PyList_Append(result, tuple) < 0)
        goto bad;
      Py_DECREF(tuple);
      tuple = 0;
    }
  }
  return result;
bad:
  Py_XDECREF(tuple);
  Py_XDECREF(result);
  return 0;
}

/*---------------------- Exported Functions -----------------------*/

/*
 *  is_abf_file(filename) -- Test if a file is an ABF file
 *  =====================
 *
 *  Return value:
 *    1 for yes, 0 for no, -1 if filename could not be parsed
 */
static PyObject *ABFInter_is_abf_file(PyObject *self, PyObject *args) {
  char *filename;
  int ans;
  PyObject *result = 0;

  if (!PyArg_ParseTuple(args, "s", &filename))
	  ans = -1;
  ans = ABF_IsABFFile(filename, NULL, NULL);
  if (ans != 1)
	  ans = 0;
  result = Py_BuildValue("i", ans);
  return result;
}

/*
 *  get_abf_info(filename) -- Return channel info from ABF file
 *  ======================
 *
 *  Return value:
 *    (channels dac_units)
 *  where
 *    channels = [(n, name, units)...]
 *      n = (0-based) logical channel number
 *      name = channel name
 *      units = measurement units string
 *    dac_units = measurement units string for DAC channel
 */

static PyObject *ABFInter_get_abf_info(PyObject *self, PyObject *args) {
  char *filename;
  ABFFile abf;
  int dac_chan;
  PyObject *channel_info = 0;
  PyObject *dac_units = 0;
  PyObject *result = 0;
  
  abf.isopen = 0;
  if (!PyArg_ParseTuple(args, "s", &filename))
    goto bad;
  if (open_abf_file(&abf, filename) < 0)
    goto bad;
  channel_info = get_channel_info(&abf);
  if (!channel_info)
    goto bad;
  dac_chan = abf.header.nActiveDACChannel;
  dac_units = PyString_FromStringAndSize(abf.header.sDACChannelUnits[dac_chan], 
    ABF_DACUNITLEN);
  result = Py_BuildValue("OO", channel_info, dac_units);
bad:
  Py_XDECREF(channel_info);
  Py_XDECREF(dac_units);
  close_abf_file(&abf);
  return result;
}

/*
 *  read_abf_file(filename, channel) -- Read data from ABF file.
 *  ================================
 *
 *  filename = name of the ABF file
 *  channel  = (0-based) logical channel number
 *
 *  Returns a tuple ([time,...], [sweep,...]) where
 *  sweep = (fit_epoch_start_sample, fit_epoch_end_sample, 
 *           fit_epoch_volt, prev_epoch_volt, samples) where
 *  samples = [sample,...].
 */
 
static PyObject *ABFInter_read_abf_file(PyObject *self, PyObject *args) {
  char *name;
  ABFFile abf;
  int log_chan, phys_chan, exp_type;
  PyObject *result;
  
  abf.isopen = 0;
  if (!PyArg_ParseTuple(args, "sii", &name, &log_chan, &exp_type))
    goto bad;
  if (open_abf_file(&abf, name) < 0)
    goto bad;
  if (debug) {
    int i;
    printf("   lNumSamplesPerEpisode: %d\n", abf.header.lNumSamplesPerEpisode);
    printf("   lEpisodesPerRun:       %d\n", abf.header.lEpisodesPerRun);
	printf("   lActualEpisodes:       %d\n", abf.header.lActualEpisodes);
    printf("   lRunsPerTrial:         %d\n", abf.header.lRunsPerTrial);
    printf("   fADCSampleInterval:    %g\n",  abf.header.fADCSampleInterval);
    printf("   fADC2ndSampleInterval: %g\n",  abf.header.fADCSecondSampleInterval);
    printf("   lClockChange:          %ld\n", abf.header.lClockChange);
    printf("   nADCNumChannels:       %hd\n", abf.header.nADCNumChannels);
    printf("   nOperationMode:        %hd\n", abf.header.nOperationMode);
    printf("   lPreTriggerSamples:    %ld\n", abf.header.lPreTriggerSamples);
    printf("   nActiveDACChannel:     %hd\n", abf.header.nActiveDACChannel);
    printf("\n");
    i = abf.header.nActiveDACChannel;
    printf("Active DAC Channel:\n");
    printf("   sDACChannelName:       '%.10s'\n", abf.header.sDACChannelName[i]);
    printf("   sDACChannelUnits:      '%.8s'\n", abf.header.sDACChannelUnits[i]);
    printf("   fDACScaleFactor:       %g\n", abf.header.fDACScaleFactor[i]);
    printf("\n");
    printf("Epoch WaveForm and Pulses:\n");
    printf("   nDigitalEnable:        %hd\n", abf.header.nDigitalEnable);
    printf("   nWaveformSource:       %hd\n", abf.header.nWaveformSource);
    printf("   nActiveDACChannel:     %hd\n", abf.header.nActiveDACChannel);
    printf("   nInterEpisodeLevel:    %hd\n", abf.header.nInterEpisodeLevel);
    printf("   nDigitalHolding:       %hd\n", abf.header.nDigitalHolding);
    printf("   nDigitalInterEpisode:  %hd\n", abf.header.nDigitalInterEpisode);
    printf("   Epoch      Type   InitLevel    LevelInc   InitDur    DurInc  DigValue\n");
    for (i = 0; i < ABF_EPOCHCOUNT; i++)
      printf("%8d%10d%12f%12f%10d%10d%10d\n", 
        i,
        abf.header.nEpochType[0][i], 
        abf.header.fEpochInitLevel[0][i],
        abf.header.fEpochLevelInc[0][i], 
        abf.header.lEpochInitDuration[0][i], 
        abf.header.lEpochDurationInc[0][i], 
        abf.header.nDigitalValue[i]);
    printf("\n");
    printf("User Parameter List:\n");
    printf("   nULParamToVary:          %d\n", abf.header.nULParamToVary[0]);
    printf("   sULParamValueList:       %s\n", abf.header.sULParamValueList[0]);
  }
  abf.sweepLength = abf.header.lNumSamplesPerEpisode / abf.header.nADCNumChannels;
  if (debug) {
    printf("+++ sweepLength = %d\n", abf.sweepLength);
  }
  if (log_chan < 0 || log_chan >= ABF_ADCCOUNT) {
    PyErr_Format(ABFError, "Logical channel number %d out of range", 
      log_chan);
    goto bad;
  }
  phys_chan = abf.header.nADCSamplingSeq[log_chan];
  result = read_channel(&abf, phys_chan, exp_type);
  goto done;
bad:
  result = 0;
done:
  close_abf_file(&abf);
  return result;
}

/*---------------------- Function Table -----------------------*/

static PyMethodDef ABFInter_methods[] = {
	{"is_abf_file", ABFInter_is_abf_file, METH_VARARGS},
	{"get_abf_info", ABFInter_get_abf_info,  METH_VARARGS},
	{"read_abf_file", ABFInter_read_abf_file,	METH_VARARGS},
	{NULL,	NULL}
};

/*---------------------- Initialisation -----------------------*/

DL_EXPORT(void)
initABFInter()
{
  PyObject *m, *d;
  m = Py_InitModule("ABFInter", ABFInter_methods);
  d = PyModule_GetDict(m);
  ABFError = PyErr_NewException("ABFInter.ABFError", NULL, NULL);
  PyDict_SetItemString(d, "ABFError", ABFError);
}