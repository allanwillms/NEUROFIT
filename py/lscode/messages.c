/*
	This file contains the definition of the messages strings.
	The order is as determined in messages.h.
*/

#include "messages.h"

const char *messages[MAXMSG] = {
	"WARNING: Noise levels unable to be calculated for all sweeps due to insufficient noise window specifications.  All noise levels set to 1.0.",
	"WARNING: Jacobian matrix is singular at the obtained solution.  Parameters which do not significantly affect the data are indicated with a zero standard error.  Try refitting with these parameters held constant.",
	"ERROR: Improper input parameters to LMDER.",
	"INFO: Successful completion.  Algorithm terminated since the relative reduction in the sum of squares was at most ftol.",
	"INFO: Successful completion.  Algorithm terminated since the relative error between two consecutive solutions was at most xtol.",
	"INFO: Successful completion.  Algorithm terminated since the derivative is essentially (as determined by gtol) zero in all directions at the current solution.",
	"WARNING: Algorithm terminated since the number of function evaluations has exceeded the maximum.  Try increasing maxfeval.",
	"WARNING: ftol is too small.  No further reduction in the sum of squares is possible.",
	"WARNING: xtol is too small.  No further improvement in the approximate solution is possible.",
	"WARNING: gtol is too small.  The derivative is essentially zero in all directions to machine precision.",
	"ERROR: The specified value of the model parameter gmax must be nonnegative.",
	"ERROR: The specified value of each of the model parameters f must be between zero and one, and their sum must be less than or equal to one.",
	"ERROR: The sum of the specified values of the model parameters f that are constant must be less than one to allow the non-constant parameters f room to vary.",
	"ERROR: The specified value of the model parameter sm must be negative.",
	"ERROR: The specified value of each of the model parameters Tm must be positive.",
	"ERROR: The specified value of the model parameter sh must be positive.",
	"ERROR: The specified value of each of the model parameters Th must be positive.",
	"ERROR: The magnitudes of the model parameters Th for each channel type must have the same ordering for each voltage step level.",
	"ERROR: One of the sweeps with a nonempty fitting window has a step potential equal to the reversal potential.  Such data has no theoretical signal.",
	"ERROR: The constraint inequalities are inconsistent.",
	"WARNING: The value for sm has been modified as it was too close to zero.",
	"WARNING: The value for sh has been modified as it was too close to zero.",
	"WARNING: A constant (non-varying) Taum value has been modified as it was too close to zero.",
	"WARNING: A constant (non-varying) Tauh value has been modified as it was too close to zero.",
	"ERROR: The specified value of the model parameter A0m must be positive.",
	"ERROR: The specified value of the model parameter a1m must be nonnegative.",
	"ERROR: The specified value of the model parameter b1m must be nonpositive.",
	"ERROR: The specified value of the model parameter a2m must be nonpositive.",
	"ERROR: The specified value of the model parameter b2m must be nonpositive.",
	"ERROR: The specified value of the model parameter A0h must be positive.",
	"ERROR: The specified value of the model parameter a1h must be nonpositive.",
	"ERROR: The specified value of the model parameter b1h must be nonnegative.",
	"ERROR: The specified value of the model parameter a2h must be nonpositive.",
	"ERROR: The specified value of the model parameter b2h must be nonpositive.",
	"ERROR: The specified value of the model parameter e0h2 must be larger than -A0h.",
	"ERROR: The specified value of the model parameter e1h2 must satisfy -b1h <= e1h2 <= -a1h.",
	"ERROR: The specified value of the model parameter e2h2 must satisfy e2h2 <= MIN(-a1h, -b1h).",
	"ERROR: The specified value of the model parameter e0h3 must be larger than -A0h.",
	"ERROR: The specified value of the model parameter e1h3 must satisfy -b1h <= e1h2 <= -a1h.",
	"ERROR: The specified value of the model parameter e2h3 must satisfy e2h2 <= MIN(-a1h, -b1h).",
};
