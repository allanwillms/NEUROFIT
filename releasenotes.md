<h2>Release Notes</h2>
<big><strong>1.7</strong></big> Released May 21, 2004.
<p>
This release fixed a bug in the voltage editor window.  It now has a scrollbar
for files with large numbers of sweeps.
<p>
<big><strong>1.6</strong></big> Released March 10, 2003.
<p>
This release fixed a number of bugs related to fitting data where
the number of inactivating channel groups, nh, is zero.  (That is,
when fitting non-inactivating currents.)  In particular, the Quick
Fit function was returning garbage for some of the parameter values,
and the Full Fit function was requesting an initial value for V2h.
This release also improved the output of some of the numbers on the screen
which occasionally overran the available space when in exponential form.
<p>
<big><strong>1.5</strong></big> Released November 20, 2002.
<p>
This release fixed a number of bugs and improved several features:
<ul>
<li>The start of the voltage step is now correctly determined from ABF files.
<li>A tutorial has been added.
<li>The fitted curves are now only computed for the time values for which
the voltage step is active.
<li>Sweep data is now graphed in different colours depending on whether it
is outside the model regime, inside but excluded, or inside and included.
<li>Zooming features for the graphs have been improved including a
pull-down menu for zooming to pre-set limits.
<li>Fixed bugs in and improved the Report generation feature.  Moved it
from the data menu to the file menu.
<li>Improved the way units are handled and included support for where the
data in different files is in different units.
<li>Several other minor bugs fixed.
</ul>
<p>
<big><strong>1.4</strong></big> Released July 1, 2002.
<p>
This release fixed a bug with regard to reading time values from ABF files
which had two different sampling intervals.
<p>
<big><strong>1.3</strong></big> Released May 29, 2002.
<p>
This release fixed a bug in the Quick Fit function which was causing
garbage to sometimes appear in the V2m and sm fields.
Also the colour of the sweep include buttons was changed from red to
green.
<p>
<big><strong>1.2</strong></big> Released February 21, 2002.
<p>
This release fixes a few minor bugs including:
<ul>
<li> The number of points fitted is now reported properly.
<li> Fitting and time windows are restricted from moving to the
left of time zero.
<li> A bug in the reading of text data files has been fixed.
</ul>
<big><strong>1.1</strong></big> Released January 28, 2002.
<p>
This release is mostly transparent to the user except for the first two
items listed below.
<ul>
<li> Changed all occurrences of the text "Neurofit" to all capitals,
including the name of the installation directory.
<li> Increased the domain of convergence, that is, initial values can
be more distant from the true values and the algorithm will still 
converge to the true values.
<li> Minor updates to a few of the help files.
</ul>
<p>
<big><strong>1.0</big></strong> Released December 20, 2001.
<p>
Original release.
