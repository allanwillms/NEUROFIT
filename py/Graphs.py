############################################################
#
#  Graphs.py - NEUROFIT, graph display module
#  =========   Greg Ewing, Jan 2001
#
############################################################

from math import log10, floor, ceil
import os
from Tkinter import *
from Layout import grid, group_label
from ModelView import View, ViewFrame, ParentChild
from Units import CanvasUnitView
from Voltages import VoltageEditor

module_dir = os.path.dirname(__file__)
image_dir = os.path.join(module_dir, "images")
stripes_image = os.path.join(image_dir, "stripes.xbm")

zoom_setting_foreground_colour = 'blue'
fit_colour = 'red'
included_data_colour = 'blue'
excluded_data_colour = 'lightblue' #'#10D0F0'
unmodellable_data_colour = 'grey'
fit_region_marker_colour = '#80FF80'
noise_region_marker_colour = '#FFF040'

zoom_click_factor = 1.5

class ExperimentView(ViewFrame):
  """Widget displaying a graph of experimental and
  plotted data for one experiment, together with controls
  for selecting sweeps to display, adjusting noise and
  fit regions, and entering sweep voltages."""

  graph_width = 330      # Size of the whole graph area, including
  graph_height = 215     # axis markings and fit/noise region handles
  
  left_margin = 50       # Distances from the edges of the graph area
  bottom_margin = 55     # to the plotting area
  right_margin = 40
  top_margin = 10
  
  tick_length = 5        # Length of the tick marks on the graph axes
  
  handle_width = 10      # Size of the handles for dragging the
  handle_height = 20     # fit/noise region handles
  
  zoom_cursor = 'target'

  experiment_names = (
    "Varying Step",
    "Varying Pre-Step"
    )

  plot_width = graph_width - left_margin - right_margin
  plot_height = graph_height - top_margin - bottom_margin
  plot_centre_x = left_margin + plot_width * 0.5
  plot_centre_y = top_margin + plot_height * 0.5
  
  def __init__(self, parent, experiment):
    ViewFrame.__init__(self, parent)
    experiment.add_view(self)
    self.plot_lines = {}           # FtSweep -> LineSet
    zeros = ((0.0, 0.0), (0.0, 0.0))
    self.axis_limits = {'all': zeros, 'step': zeros, 'current': zeros,\
                        'custom': None}
    self.zooming = 0
    self.transform = Transform()
    self.voltage_editor = None # a VoltageEditor
    head = group_label(
      self, self.experiment_names[experiment.experiment_number])
    # Create canvas to hold the plotting area, graph axes and scales,
    # and the fit/noise region control handles.
    gcanv = self.graph_canvas = Canvas(self, 
      width = self.graph_width, height = self.graph_height, 
      background = 'white')
    # Create subcanvas to use as the plotting area. Set its coordinate
    # system to be the same as the graph canvas.
    pcanv = self.plot_canvas = Canvas(gcanv, 
      width = self.plot_width, height = self.plot_height,
        scrollregion = (self.left_margin, self.top_margin,
        self.left_margin + self.plot_width,
        self.top_margin + self.plot_height),
      background = 'white', cursor = self.zoom_cursor,
      highlightthickness = 0)
    gcanv.create_window(self.left_margin, self.top_margin, 
      window = pcanv, anchor = 'nw')
    pcanv.bind('<Button>', self.begin_zoom_rect)
    pcanv.bind('<Motion>', self.track_zoom_rect)
    pcanv.bind('<ButtonRelease>', self.end_zoom_rect)
    pcanv.bind('<Double-Button-1>', self.reset_zoom_step)
    pcanv.bind('<Double-Button-2>', self.reset_zoom_all)
    pcanv.bind('<Double-Button-3>', self.reset_zoom_all)
    pcanv.bind('<Control-Button>', self.define_step_epoch)
    # Create unit labels
    vaxis_unit = CanvasUnitView(self, gcanv, experiment.parent.units['Current'],
      5, self.plot_centre_y, anchor = 'w')
    haxis_unit = CanvasUnitView(self, gcanv, experiment.parent.units['Time'],
      self.plot_centre_x,
      self.top_margin + self.plot_height + self.tick_length + 15,
      anchor = 'n')
    # Create region controllers
    self.make_region_controllers()
    # Create buttons
    bvolt = Button(self, text="Voltages...", command = self.open_voltage_editor)
    baxes = Menubutton(self, text="Zoom", borderwidth=2, relief="raised", indicatoron=1)
    self.zoom_axes_menu = Menu(baxes, tearoff=0)
    baxes.configure(menu = self.zoom_axes_menu)
    self.zoom_axes_menu.add_command(label = "to step data", command = self.reset_zoom_step)
    self.zoom_axes_menu.add_command(label = "to all data", command = self.reset_zoom_all)
    self.zoom_axes_menu.add_command(label = "to Custom", command = self.reset_zoom_custom, state="disabled")
    self.zoom_axes_menu.add_separator()
    self.zoom_axes_menu.add_command(label = "Set Custom", command = self.set_custom_axis_limits)
    fswp = self.sweep_list = SweepList(self)
    fswp.sticky = 'n'
    grid([head, bvolt, baxes, baxes],
         [fswp, fswp, gcanv, gcanv])
    self.currently_displayed = self.model  # tells what is currently displayed, experiment, sweep_set or sweep
    self.update()

  def make_region_controllers(self):
    self.fit_region_controller = RegionController(
      self, fit_region_marker_colour, 'fit')
    self.noise_region_controller = RegionController(
      self, noise_region_marker_colour, 'noise')

  def experiment(self):
    return self.model # an FtExperiment

  def destroy(self):
    self.close_voltage_editor()
    ViewFrame.destroy(self)

  def update(self):
    """Redisplay everything."""
    self.currently_displayed = self.experiment()
    self.close_voltage_editor()
    self.update_axis_limits()
    self.update_scales()
    self.set_current_regions(self.currently_displayed)
    self.update_region_controllers()
    self.update_lines()
    self.update_sweep_list()

  def update_after_step_epoch_change(self):
    self.update_axis_limits()
    self.update_scales()
    self.set_current_regions(self.currently_displayed)
    self.update_region_controllers()
    self.update_lines()
    self.select_sweeps(self.currently_displayed)

  def update_region_controllers(self):
    self.fit_region_controller.update()
    self.noise_region_controller.update()
    
  def update_lines(self):
    """Redisplay all lines."""
    experiment = self.experiment()
    for lineset in self.plot_lines.values():
      lineset.destroy()
    self.plot_lines = {}
    self.display_axes()
    for set in experiment.sweep_sets:
      times = set.times
      for sweep in set.sweeps:
        linedefs = {}
        origin = times[sweep.step_epoch_ends[0]]
        colour = unmodellable_data_colour
        line = self.plot_data(times[0:sweep.step_epoch_ends[0]], origin,\
                              sweep.sweep_data[0:sweep.step_epoch_ends[0]], colour)
        linedefs['left_outside_data'] = [line, colour]
        line = self.plot_data(times[sweep.step_epoch_ends[1]+1:-1], origin,\
                              sweep.sweep_data[sweep.step_epoch_ends[1]+1:-1], colour)
        linedefs['right_outside_data'] = [line, colour]
        if sweep.fitting_flag:
          colour = included_data_colour
        else:
          colour = excluded_data_colour
        line = self.plot_data(times[sweep.step_epoch_ends[0]:sweep.step_epoch_ends[1]+1],\
                              origin,\
                              sweep.sweep_data[sweep.step_epoch_ends[0]:sweep.step_epoch_ends[1]+1],\
                              colour)
        linedefs['inside_data'] = [line, colour]
        colour = fit_colour
        line = self.plot_data(times[sweep.step_epoch_ends[0]:sweep.step_epoch_ends[1]+1],\
                              origin, sweep.fitted_data, colour)
        linedefs['fitted_line'] = [line, colour]
        self.plot_lines[sweep] = LineSet(self, sweep, self.plot_canvas, linedefs)

  def update_fitted_lines(self):
    """Redisplay lines for fitted data only."""
    for set in self.experiment().sweep_sets:
      times = set.times
      for sweep in set.sweeps:
        origin = times[sweep.step_epoch_ends[0]]
        line_set = self.plot_lines[sweep]
        line, colour = line_set.linedefs['fitted_line']
        if line:
          self.plot_canvas.delete(line)
        line = self.plot_data(times[sweep.step_epoch_ends[0]:sweep.step_epoch_ends[1]+1],\
                              origin, sweep.fitted_data, colour)
        line_set.linedefs['fitted_line'] = [line, colour]
        line_set.update()

  def plot_data(self, times, time_shift, values, colour):
    points = self.transform.data_to_canvas(times, time_shift, values)
    if len(points) > 1:
      line = self.plot_canvas.create_line(points, fill = colour, tag = 'line')
      return line
    else:
      return None

  def update_sweep_list(self):
    self.sweep_list.update()

  def update_axis_limits(self):
    """Calculate the all and step axis limits and re-assign the current limits
    to the step limits.
    """
    experiment = self.experiment()
    # transfer custom limits if any were read from project file
    file_custom_limits = experiment.custom_axis_limits_to_transfer
    if file_custom_limits is not None:
      self.set_custom_axis_limits(file_custom_limits)
      experiment.custom_axis_limits_to_transfer = None
    for key in experiment.data_range.keys():  # keys = step, all
      limits = []
      for data_range in experiment.data_range[key]:
        extra = 0.04*(data_range[1] - data_range[0])
        limits.append((data_range[0] - extra, data_range[1] + extra))
      self.axis_limits[key] = tuple(limits)
    self.axis_limits['current'] = self.axis_limits['step']
    self.reset_zoom_menu(0)

  def scale_axis_limits(self, name, factor):
    """Called when time project units are scaled."""
    if name == 'Time':
      for key in self.axis_limits.keys():
        r = self.axis_limits[key]
        if r is not None:
          self.axis_limits[key] = ((r[0][0] * factor, r[0][1] * factor), r[1])
    elif name == 'Current':
      for key in self.axis_limits.keys():
        r = self.axis_limits[key]
        if r is not None:
          self.axis_limits[key] = (r[0], (r[1][0] * factor, r[1][1] * factor))
  
  def set_custom_axis_limits(self, limits = None):
    self.zoom_axes_menu.entryconfigure(2, state = "normal",
                                       foreground = zoom_setting_foreground_colour)
    if limits is None:
      limits = self.axis_limits['current']
    self.axis_limits['custom'] = limits

  def update_scales(self):
    """Calculate transformation between data and plot_canvas coordinates."""
    canvdim = [float(self.plot_width), float(self.plot_height)]
    canvoffset = [self.left_margin, self.top_margin + self.plot_height]
    limits = self.axis_limits['current']
    scale = []
    origin = []
    for i in range(0,2):
      dist = pow(-1,i)*(limits[i][1] - limits[i][0])  # -1 for y due to down being positive
      if dist != 0.0:
        scale.append(canvdim[i] / dist)
      else:
        scale.append(0.0)
      origin.append(canvoffset[i] - limits[i][0] * scale[i])
    self.transform.set(origin = (origin[0], origin[1]), scale = (scale[0], scale[1]))

  def display_axes(self):
    """Display axes and labels."""
    c = self.graph_canvas
    tr = self.transform
    x0 = self.left_margin - 1
    y0 = self.graph_height - self.bottom_margin
    x1 = self.graph_width - self.right_margin
    y1 = self.top_margin
    tl = self.tick_length
    c.delete('axes')
    c.create_line(x0, y0, x1, y0, tag = 'axes')
    c.create_line(x0, y0, x0, y1, tag = 'axes')
    num_xticks = 5
    num_yticks = 5
    dx0, dy0 = tr.canvas_to_data_pt((x0, y0))
    dx1, dy1 = tr.canvas_to_data_pt((x1, y1))
    tick_spacing, first_tick, last_tick = self.calc_ticks(dx0, dx1, num_xticks)
    for i in range(first_tick, last_tick + 1):
      data_x = i * tick_spacing
      x = tr.data_to_canvas_x(data_x)
      c.create_line(x, y0, x, y0 + tl, tag = 'axes')
      label = self.format_axis_label(data_x)
      c.create_text(x, y0 + tl + 2, text = label, anchor = 'n', tag = 'axes')
    tick_spacing, first_tick, last_tick = self.calc_ticks(dy0, dy1, num_yticks)
    for i in range(first_tick, last_tick + 1):
      data_y = i * tick_spacing
      y = tr.data_to_canvas_y(data_y)
      c.create_line(x0, y, x0 - tl, y, tag = 'axes')
      label = self.format_axis_label(data_y)
      c.create_text(x0 - tl - 2, y, text = label, anchor = 'e', tag = 'axes')
  
  def calc_ticks(self, d0, d1, num_ticks):
    """
    Figure out a reasonable distribution of ticks for data range
    d0...d1 and suggested number of ticks num_ticks.
    """
    #print "ExperimentView.calc_ticks:", d0, d1, num_ticks ###
    # Take a first stab at the tick spacing
    init_tick_spacing = (d1 - d0) / num_ticks
    #print "...init_tick_spacing =", repr(init_tick_spacing) ###
    if init_tick_spacing == 0.0:
      return 0.0, 0, 0
    # Figure out order of magnitude
    mag = int(floor(log10(init_tick_spacing)))
    #print "...mag =", mag ###
    # Try 1, 2, 5 * a power of 10 until we find something not
    # smaller than the initial guess
    power = 10.0 ** mag
    #print "...power =", repr(power) ###
    for mult in (1, 2, 5, 10):
      tick_spacing = mult * power
      #print "...trying tick_spacing =", repr(tick_spacing) ###
      if tick_spacing >= init_tick_spacing:
        break
    # Find indexes of first and last tick in data range
    first_tick = int(ceil(d0 / tick_spacing))
    last_tick = int(floor(d1 / tick_spacing))
    #print "...first_tick, last_tick =", first_tick, last_tick ###
    return tick_spacing, first_tick, last_tick
  
  def format_axis_label(self, value):
    """Turn an x or y value into a string suitable for use as
    a label for an axis tick mark."""
    # Round to 3 sig figs
    value = float("%.3g" % value)
    if 0.001 <= abs(value) < 10000:
      label = str(value)
      # Remove any trailing ".0"
      if label[-2:] == ".0":
        label = label[:-2]
    else:
      label = "%.3g" % value
      if 'e' in label:
        # Remove leading zeroes and '+' sign from exponent
        mant, exp = label.split("e")
        label = "%se%s" % (mant, int(exp))
    return label

  def plot_canvas_event_coords(self, event):
    """
    Transform the coordinates from a mouse event in the
    plot_canvas to canvas coordinates.
    """
    x = event.x + self.left_margin
    y = event.y + self.top_margin
    return x, y
  
  def define_step_epoch(self, event):  
    """
    Define the limits of the step epoch.
    """
    if event.num == 1:  # button 1 is left end, others are right end
      end = 0
    else:
      end = 1
    x, y = self.plot_canvas_event_coords(event)
    t = self.transform.canvas_to_data_x(x)
    self.experiment().define_step_epoch(t, self.currently_displayed, end)

  def begin_zoom_rect(self, event):
    """
    Start dragging a rectangle to zoom in to.
    """
    #print "ExperimentView.begin_zoom_rect:", event ###
    x, y = self.plot_canvas_event_coords(event)
    self.zoom_corner_1 = (x, y)
    self.zoom_rubber_rect = self.plot_canvas.create_rectangle(x, y, x, y)
    self.zooming = 1
  
  def track_zoom_rect(self, event):
    """
    Adjust size of zoom-in rectangle while mouse is dragged.
    """
    #print "ExperimentView.track_zoom_rect:", event ###
    if self.zooming:
      corner1 = self.zoom_corner_1
      corner2 = self.plot_canvas_event_coords(event)
      self.plot_canvas.coords(self.zoom_rubber_rect, corner1 + corner2)
  
  def end_zoom_rect(self, event):
    """
    Handle release of mouse after dragging out a zoom rectangle.
    """
    if self.zooming:
      if event.num == 1:
        direction = 'in'
      else:
        direction = 'out'
      corner1 = self.zoom_corner_1
      corner2 = self.plot_canvas_event_coords(event)
      if self.zoom_rubber_rect:
        self.plot_canvas.delete(self.zoom_rubber_rect)
        self.zoom_rubber_rect = None
      centre, factor = self.zoom_calc_trans(corner1, corner2, direction)
      limits = self.zoom_calc_limits(centre, factor)
      self.reset_zoom_menu(-1)
      self.zoom_apply(centre, factor, limits)
      self.zooming = 0

  def zoom_calc_trans(self, corner1, corner2, direction):
    """
    Calculate the centre of the zoom box in canvas coords and
    determine the zoom factor.
    If direction is 'in', expand the region enclosed by the 
    given rectangle to fill the plot_canvas.
    If direction is 'out', contract the region covered by the
    plot canvas to fill the given rectangle.
    If the rectangle is empty, zoom in or out by a factor
    of zoom_click_factor.
    """
    # Compute the scale factors and canvas coord centres for each dimension.
    if corner1[0] == corner2[0] or corner1[1] == corner2[1]:
      centre = corner1
      if direction == 'in':
        factor = zoom_click_factor
      else:
        factor = 1.0 / zoom_click_factor
      factor = (factor, factor)  # two copies, one for x, one for y
    else:
      canvdim = [float(self.plot_width), float(self.plot_height)]
      dist = []
      centre = []
      factor = []
      for i in range(0,2):
        dist.append(abs(corner2[i] - corner1[i]))
        centre.append(min(corner1[i], corner2[i]) + 0.5*dist[i])
        if direction == 'in':
          factor.append(canvdim[i] / dist[i])
        else:
          factor.append(dist[i] / canvdim[i])
    return centre, factor

  def zoom_calc_limits(self, centre, factor):
    """
    Calculate the new axis limits for the current zoom settings.
    """
    # Compute the new limits in data coords.
    data_centre = self.transform.canvas_to_data_pt(centre)
    limits = []
    for i in range(0,2):
      currlimits = self.axis_limits['current'][i]
      l_range = currlimits[1] - currlimits[0]
      halfdist = 0.5 * l_range / factor[i]
      limits.append((data_centre[i] - halfdist, data_centre[i] + halfdist))
    return limits

  def zoom_apply(self, centre, factor, limits):
    """
    Apply the zoom transformation to the graphs and update everything.
    """
    old_centre = [self.plot_centre_x, self.plot_centre_y]
    delta = []
    for i in range(0,2):
      delta.append(old_centre[i] - centre[i])
    self.plot_canvas.move('line', delta[0], delta[1])
    self.plot_canvas.scale('line', self.plot_centre_x, self.plot_centre_y, factor[0], factor[1])
    self.axis_limits['current'] = tuple(limits)
    self.update_scales()
    self.display_axes()
    self.update_region_controllers()

  def reset_zoom_step(self, event=None):
    self.reset_zoom_menu(0)
    self.reset_zoom(event, self.axis_limits['step'])

  def reset_zoom_all(self, event=None):
    self.reset_zoom_menu(1)
    self.reset_zoom(event, self.axis_limits['all'])

  def reset_zoom_custom(self, event=None):
    if self.axis_limits['custom'] is not None:
      self.reset_zoom_menu(2)
      self.reset_zoom(event, self.axis_limits['custom'])

  def reset_zoom(self, event, new_limits):
    """
    Reset zooming parameters to show preset region.
    """
    curr_limits = self.axis_limits['current']
    pt = []
    factor = []
    for i in range(0,2):
      new_dist = new_limits[i][1] - new_limits[i][0]
      pt.append(new_limits[i][0] + 0.5*new_dist)
      if new_dist != 0.0:
        factor.append((curr_limits[i][1] - curr_limits[i][0])/new_dist)
      else:
        factor.append(1.0)
    centre = self.transform.data_to_canvas_pt(pt)
    self.zoom_apply(centre, factor, new_limits)

  def reset_zoom_menu(self, selecteditem):
    colours = ('black', zoom_setting_foreground_colour)
    for i in range(3):
      self.zoom_axes_menu.entryconfigure(i, foreground = colours[i == selecteditem])

  def open_voltage_editor(self):
    if not self.voltage_editor:
      self.voltage_editor = VoltageEditor(self, self.experiment())
    self.voltage_editor.tkraise()

  def close_voltage_editor(self):
    if self.voltage_editor:
      self.voltage_editor.destroy()

  def voltages_changed(self):
    self.update_sweep_list()

  def select_sweeps(self, target):
    self.currently_displayed = target
    self.set_current_regions(target)
    if target is self.experiment():
      self.show_all_lines(1)
    else:
      self.show_all_lines(0)
      if target in self.experiment().sweep_sets:
        sweeps = target.sweeps
      else:
        sweeps = [target]
      for sweep in sweeps:
        self.plot_lines[sweep].show(1)

  def set_current_regions(self, target):
    self.fit_region_controller.set_region(target.fit_region)
    self.noise_region_controller.set_region(target.noise_region)
    self.sweep_list.highlight_selection_bar_for(target)

  def show_all_lines(self, state):
    for line_set in self.plot_lines.values():
      line_set.show(state)

#----------------------------------------------------------

class FastRectangle:
  """
  An attempt at making the rubber rectangle less
  sluggish, which didn't work. Not currently used.
  """

  def __init__(self, canvas, x1, y1, x2, y2):
    pass
  
  def coords(self, ((x1, y1), (x2, y2))):
    pass
  
  def delete(self):
    pass
  
#----------------------------------------------------------

class Transform:
  """Manager of transformations between data values and canvas
  coordinates."""

  def set(self, origin, scale):
    self.x_origin, self.y_origin = origin
    self.x_scale, self.y_scale = scale

  def data_to_canvas_x(self, xd):
    xc = self.x_origin + xd * self.x_scale
    #print xd, "-->", xc ###
    return xc

  def data_to_canvas_y(self, yd):
    yc = self.y_origin + yd * self.y_scale
    #print yd, "-->", yc ###
    return yc
  
  def data_to_canvas_pt(self, (xd, yd)):
    return self.data_to_canvas_x(xd), self.data_to_canvas_y(yd)
  
  #def data_to_canvas_vector(self, (xd, yd)):
  #  xc = xd * self.x_scale
  #  yc = yd * self.y_scale
  #  return xc, yc

  def canvas_to_data_x(self, xc):
    #print "Transform.canvas_to_data_x:", xc ###
    #print "...x_origin =", self.x_origin ###
    #print "...x_scale =", self.x_scale ###
    scale = self.x_scale
    if scale:
      xd = (xc - self.x_origin) / self.x_scale
    else:
      xd = 0.0
    #print xd, "<--", xc ###
    return xd

  def canvas_to_data_y(self, yc):
    scale = self.y_scale
    if scale:
      yd = (yc - self.y_origin) / self.y_scale
    else:
      yd = 0.0
    #print yd, "<--", yc ###
    return yd

  def canvas_to_data_pt(self, (xc, yc)):
    return self.canvas_to_data_x(xc), self.canvas_to_data_y(yc)
  
  def data_to_canvas(self, times, time_shift, values):
    x0 = self.x_origin
    y0 = self.y_origin
    x_scale = self.x_scale
    y_scale = self.y_scale
    points = []
    for i in xrange(len(values)):
      x = x0 + (times[i] - time_shift) * x_scale
      y = y0 + values[i] * y_scale
      points.append((x, y))
    return points
  
#----------------------------------------------------------

class LineSet(View):

  def __init__(self, parent, sweep, canvas, linedefs):
    View.__init__(self, parent)
    self.plot_canvas = canvas
    self.linedefs = linedefs  # dictionary of [line, colour] lists keyed by name
         # names are: fitted_line, inside_data, left_outside_data, and right_outside_data
    self.visible = 1
    self.set_model(sweep)

  def destroy(self):
    for linedef in self.linedefs.values():
      self.plot_canvas.delete(linedef[0])
    View.destroy(self)

  def fitting_flag_changed(self, fitting_flag):
    if fitting_flag:
      dcolour = included_data_colour
      fcolour = fit_colour
    else:
      dcolour = excluded_data_colour
      fcolour = ''
    line = self.linedefs['inside_data']
    line[1] = dcolour
    self.show_line(line[0], line[1])
    line = self.linedefs['fitted_line']
    line[1] = fcolour
    self.show_line(line[0], line[1])
    
  def show(self, state):
    self.visible = state
    self.update()

  def update(self):
    for linedef in self.linedefs.values():
      self.show_line(linedef[0], linedef[1])
    
  def show_line(self, line, colour):
    if line:
      if self.visible:
        self.plot_canvas.itemconfigure(line, fill = colour)
      else:
        self.plot_canvas.itemconfigure(line, fill = '')
    
#----------------------------------------------------------

class RegionController(ParentChild):
  """Controls for displaying and setting a noise or fit region."""

  def __init__(self, exp_view, colour, type):
    ParentChild.__init__(self, exp_view)
    self.region = None
    self.transform = exp_view.transform
    if type == 'fit':
      handle_width = exp_view.handle_width
      y_offset = exp_view.handle_height + 3
    else:
      handle_width = exp_view.handle_width / 2
      y_offset = 0
    self.markers = (
      RegionMarker(self, exp_view, 0, colour, handle_width),
      RegionMarker(self, exp_view, 1, colour, handle_width))
    self.def_reg_button = self.make_default_region_button(exp_view, colour, y_offset)
    #for i in (0, 1):
    #  self.markers[i].other_graphic = self.markers[1-i].graphic

  def make_default_region_button(self, exp_view, col, y_offset):
    """Make the graphic button which will revert the region to its
    default setting upon a click. """
    gcanv = exp_view.graph_canvas
    x0 = 3
    x1 = exp_view.left_margin - 15
    y = exp_view.graph_height - 1 - y_offset
    w = exp_view.handle_width
    h = exp_view.handle_height
    rect = gcanv.create_rectangle(x0 - 1, y - h - 1, x1 + 1, y + 1, fill = 'white',
                                  outline = col)
    gcanv.create_polygon(x0, y, x0, y - w, x0 + w, y - h, x0 + w, y,
                              fill = col, outline = col)
    gcanv.create_polygon(x1, y, x1, y - w, x1 - w, y - h, x1 - w, y,
                              fill = col, outline = col)
    gcanv.create_text((x0 + x1) / 2, y - h / 2, anchor = 'center',
                           text = 'reset')
    tagid = 'defreg_button' + col
    gcanv.addtag_enclosed(tagid, x0 - 1, y - h, x1 + 1, y)
    gcanv.tag_bind(tagid, sequence = '<Button>', func = self.revert)
    return rect

  def set_region(self, region):
    self.region = region
    for marker in self.markers:
      marker.set_region(region)

  def revert(self, event):
    self.region.revert()

  def update(self):
    for marker in self.markers:
      marker.update()

class RegionMarker(View):
  """Draggable marker for setting min or max value of
  a noise or fit region."""

  def __init__(self, parent, exp_view, end, colour, handle_width):
    View.__init__(self, parent)
    self.transform = exp_view.transform   # a Transform
    self.graph_canvas = exp_view.graph_canvas
    self.plot_canvas = exp_view.plot_canvas
    self.colour = colour
    x0 = exp_view.left_margin
    x1 = exp_view.graph_width - exp_view.right_margin
    y0 = exp_view.top_margin
    y1 = exp_view.graph_height - exp_view.bottom_margin
    self.xlimits = (x0, x1)
    self.xmin = x0
    self.xmax = x1
    self.region = None
    self.end = end
    self.x = x0
    w = handle_width
    s = (-1, 1)[end] * w
    ht = exp_view.graph_height - exp_view.handle_height
    hb = exp_view.graph_height
    self.handle_graphic = self.graph_canvas.create_polygon(
        x0, hb, x0, ht, x0+s, ht+w, x0+s, hb,
        fill = colour, outline = colour)
    self.hairline_graphic = self.plot_canvas.create_rectangle(
        x0-1+end, y0, x0+end, y1, outline = "",
        fill = colour)
    self.graph_canvas.tag_bind(self.handle_graphic,
        sequence = "<Button-1>", func = self.click)
    self.graph_canvas.tag_bind(self.handle_graphic,
        sequence = "<Button1-Motion>", func = self.drag)
    self.graph_canvas.tag_bind(self.handle_graphic,
        sequence = "<ButtonRelease-1>", func = self.release)

  def set_region(self, region):
    self.region = region
    self.set_model(region)

  def update(self):
    t = self.region[self.end]
    x = round(self.transform.data_to_canvas_x(t))
    self.move_graphic_to(x)
    self.set_defined(self.region.is_set())
    xmin, xmax = self.xlimits
    # Limit movement according to other end of region
    t = self.region[1 - self.end]
    x = round(self.transform.data_to_canvas_x(t))
    if self.end:
      xmin = max(xmin, x)
    else:
      xmax = min(xmax, x)
    # Limit movement to within the region limits.
    if self.region.limiting_region is not None:
      t = self.region.limiting_region[0]
      x = self.transform.data_to_canvas_x(t)
      xmin = max(xmin, x)
      t = self.region.limiting_region[1]
      x = self.transform.data_to_canvas_x(t)
      xmax = min(xmax, x)
    self.xmin = xmin
    self.xmax = xmax

  def set_defined(self, flag):
    if flag:
      self.graph_canvas.itemconfigure(self.handle_graphic, outline = 'black')
      self.graph_canvas.itemconfigure(self.parent.def_reg_button, outline = 'black')
    else:
      self.graph_canvas.itemconfigure(self.handle_graphic, outline = self.colour)
      self.graph_canvas.itemconfigure(self.parent.def_reg_button, outline = self.colour)

  def move_graphic_to(self, x):
    dx = x - self.x
    self.graph_canvas.move(self.handle_graphic, dx, 0)
    self.plot_canvas.move(self.hairline_graphic, dx, 0)
    self.x = x

  def click(self, event):
    self.mouse_offset = event.x - self.x
    return 'break'

  def drag(self, event):
    new_x = event.x - self.mouse_offset
    if new_x < self.xmin:
      new_x = self.xmin
    if new_x > self.xmax:
      new_x = self.xmax
    if new_x != self.x:
      self.move_graphic_to(new_x)
  
  def release(self, event):
    if self.region:
      self.region[self.end] = self.transform.canvas_to_data_x(self.x)

#----------------------------------------------------------

class SweepList(ViewFrame):
  """
  A display of sweeps and their voltages, with controls for
  selecting sweeps and sweep sets.
  """

  Vhold_labels = ("Prestep", "Step")
  width = 200
  height = 215

  def __init__(self, experiment_view):
    ViewFrame.__init__(self, experiment_view)
    self.voltage_unit_view = None
    self.current_unit_view = None
    self.selection_bars = []
    self.fitting_toggles = []
    self.number_displays = []
    canv = self.canvas = Canvas(self, 
      width = self.width, height = self.height,
      background = 'white')
    sbar = Scrollbar(self, orient = 'v', command = canv.yview)
    sbar.sticky = 'ns'
    canv.configure(yscrollcommand = sbar.set)
    grid([canv, sbar])

  def experiment_view(self):
    return self.parent

  def update(self):
    if self.voltage_unit_view:
      self.voltage_unit_view.destroy()
    if self.current_unit_view:
      self.current_unit_view.destroy()
    for bar in self.selection_bars:
      bar.destroy()
    self.selection_bars = []
    for toggle in self.fitting_toggles:
      toggle.destroy()
    self.fitting_toggles = []
    for d in self.number_displays:
      d.destroy()
    self.number_displays = []
    expview = self.experiment_view()
    experiment = expview.experiment()
    project = experiment.parent
    c = self.canvas
    c.delete('all')
    y = 10
    # Column headings
    c.create_text(18, y, text = "File", anchor = 'nw')
    c.create_text(105, y, text = "Voltage", anchor = 'ne')
    c.create_text(155, y, text = "Noise", anchor = 'ne')
    c.create_text(195, y, text = "Var%", anchor = 'ne')
    y = y + 12
    self.voltage_unit_view = CanvasUnitView(self, c,
        project.units['Voltage'], 105, y, anchor = 'ne')
    self.current_unit_view = CanvasUnitView(self, c,
        project.units['Current'], 155, y, anchor = 'ne')
    y = y + 10
    y0 = y + 5
    # Sweep sets
    for set in experiment.sweep_sets:
      # Sweep set name
      y = y + 5
      dir, name = os.path.split(set.path)
      c.create_text(18, y, text = name + ":", anchor = 'nw')
      y = y + 15
      y1 = y
      # Hold voltage
      lbl = self.Vhold_labels[experiment.experiment_number]
      vstr = self.voltage_string(set.Vhold)
      c.create_text(28, y, text = lbl, anchor = 'nw')
      c.create_text(105, y, text = vstr, anchor = 'ne')
      y = y + 15
      # Sweeps
      j = 1
      for sweep in set.sweeps:
        # Fitting toggle
        toggle = SweepFittingToggle(self, 40, y, sweep)
        self.fitting_toggles.append(toggle)
        # Sweep number and voltage
        y2 = y
        c.create_text(50, y, text = str(j), anchor = 'nw')
        vstr = self.voltage_string(sweep.Vsweep)
        c.create_text(105, y, text = vstr, anchor = 'ne')
        # Noise level and variance contribution
        nd = CanvasNumberDisplay(self, c, 155, y, sweep.noise)
        vcd = CanvasNumberDisplay(self, c, 195, y, sweep.varcont)
        self.number_displays.append(nd)
        self.number_displays.append(vcd)
        y = y + 15
        # Sweep selection bar
        bar = SelectionBar(self, sweep, 25, y2, y-3)
        self.selection_bars.append(bar)
        j = j + 1
      # Sweep set selection bar
      bar = SelectionBar(self, set, 15, y1, y-3)
      self.selection_bars.append(bar)
    # Select-all bar
    bar = SelectionBar(self, experiment, 5, y0, y-3)
    self.selection_bars.append(bar)
    bar.highlight(1)
    c.configure(scrollregion = (0, 0, self.width, max(y, self.height)))

  def voltage_string(self, v):
    if v is not None:
      return "%g" % v
    else:
      return "?"

  def highlight_selection_bar_for(self, target):
    for bar in self.selection_bars:
      if bar.target is target:
        bar.highlight(1)
      else:
        bar.highlight(0)

#----------------------------------------------------------

class SelectionBar(View):

  def __init__(self, sweep_list, target, x, y0, y1):
    View.__init__(self, sweep_list)
    self.canvas = sweep_list.canvas
    self.background = self.canvas.create_rectangle(
       x+1, y0+1, x+8, y1-1)
    self.foreground = self.canvas.create_rectangle(
       x+1, y0+1, x+8, y1-1, outline = 'black')
    self.target = target # an FtExperiment, FtSweepSet or FtSweep
    target.fit_region.add_view(self)
    target.noise_region.add_view(self)
    self.canvas.tag_bind(self.foreground, sequence = "<Button-1>",
       func = self.click)
    self.canvas.tag_bind(self.background, sequence = "<Button-1>",
       func = self.click)
    self.highlight(0)
    self.update()

  def experiment_view(self):
    return self.parent.experiment_view()

  def highlight(self, on):
    if on:
      fill = 'orange'
    else:
      fill = 'white'
    self.canvas.itemconfigure(self.background, fill = fill)

  def update(self):
    if self.target.fit_region.is_set() or self.target.noise_region.is_set():
      self.canvas.itemconfigure(self.foreground, fill = 'black',
         stipple = '@' + stripes_image)
    else:
      self.canvas.itemconfigure(self.foreground, fill = '')

  def click(self, *args):
    self.experiment_view().select_sweeps(self.target)

#----------------------------------------------------------

class SweepFittingToggle(View):
  """
  Control for enabling or disabling fitting of a sweep.
  """
  
  def __init__(self, parent, x, y, sweep):
    View.__init__(self, parent)
    self.canvas = parent.canvas
    self.graphic = self.canvas.create_oval(x, y+3, x+6, y+9, outline = "black")
    self.canvas.tag_bind(self.graphic, sequence = '<Button-1>',
       func = self.click)
    self.set_model(sweep)
  
  def destroy(self):
    self.canvas.delete(self.graphic)
    View.destroy(self)
  
  def fitting_flag_changed(self, fitting_flag):
    self.update()
  
  def update(self):
    if self.model.fitting_flag:
      fill = included_data_colour # green is this: "#80E000"
    else:
      fill = excluded_data_colour
    self.canvas.itemconfigure(self.graphic, fill = fill)
  
  def click(self, event):
    self.model.set_fitting_flag(not self.model.fitting_flag)

#----------------------------------------------------------

class CanvasNumberDisplay(View):
  """A canvas item displaying a view of an FtValue."""

  def __init__(self, parent, canvas, x, y, ftvalue):
    View.__init__(self, parent)
    self.canvas = canvas
    self.item = self.canvas.create_text(x, y, anchor = 'ne')
    ftvalue.add_view(self)
    self.update()

  def destroy(self):
    self.canvas.delete(self.item)
    self.item = None
    View.destroy(self)

  def update(self):
    x = self.model.value
    if x is None:
      s = ""
    else:
      s = "%.3g" % x
    self.canvas.itemconfigure(self.item, text = s)
