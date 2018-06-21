############################################################
#
#  Busy.py - NEUROFIT busy cursor module
#  =======   Greg Ewing, Dec 2001
#
############################################################

import Root

def with_busy_cursor(thunk):
  """
  Execute the thunk with the cursor changed to a watch.
  """
  root = Root.get_root()
  # Traverse widget hierarchy changing all custom cursors
  # to watches. Remember what to change back afterwards.
  changed = []
  for toplevel in root.children.values():
    install_watches(toplevel, changed, 1)
    toplevel.update()
  try:
    thunk()
  finally:
    for widget, old_cursor in changed:
      try:
        widget.configure(cursor = old_cursor)
      except:
        pass

def install_watches(widget, changed, is_toplevel):
  old_cursor = widget['cursor']
  if is_toplevel or old_cursor:
    changed.append((widget, old_cursor))
    widget.configure(cursor = 'watch')
  for child in widget.children.values():
    install_watches(child, changed, 0)
