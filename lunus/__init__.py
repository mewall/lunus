from __future__ import division
print "in lunus __init__"
import boost.python
ext = boost.python.import_ext("lunus_ext")
from lunus_ext import *
