############################################################
#
#  PathUtils.py - NEUROFIT pathname utilities module
#  ============   Greg Ewing, Sep 2001
#
############################################################

import os

def relative_path(base, target):
  """Given two absolute pathnames base and target,
  returns a relative pathname to target from the
  directory specified by base. If there is no common
  prefix, returns the target path unchanged.
  """
  common, base_tail, target_tail = split_common(base, target)
  #print "common:", common ###
  #print "base_tail:", base_tail ###
  #print "target_tail:", target_tail ###
  r = len(base_tail) * [os.pardir] + target_tail
  if r:
    return os.path.join(*r)
  else:
    return os.curdir

def split_common(path1, path2):
  """Return a tuple of three lists of pathname components:
  the common directory prefix of path1 and path2, the remaining
  part of path1, and the remaining part of path2.
  """
  p1 = split_all(path1)
  p2 = split_all(path2)
  c = []
  i = 0
  imax = min(len(p1), len(p2))
  while i < imax:
    if os.path.normcase(p1[i]) == os.path.normcase(p2[i]):
      c.append(p1[i])
    else:
      break
    i = i + 1
  return c, p1[i:], p2[i:]

def split_all(path):
  """Return a list of the pathname components of the given path.
  """
  result = []
  head = path
  while head:
    head2, tail = os.path.split(head)
    if head2 == head:
      break # reached root on Unix or drive specification on Windows
    head = head2
    result.insert(0, tail)
  if head:
    result.insert(0, head)
  return result

  
