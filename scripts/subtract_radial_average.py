from __future__ import print_function
import future,six
import lunus
import dxtbx
from dxtbx.format.FormatCBFMini import FormatCBFMini
from dxtbx.format.cbf_writer import FullCBFWriter
from dials.array_family import flex
from lunus import LunusDIFFIMAGE
import os
import numpy as np

import sys

args = sys.argv[1:] 

# Input image file
try:
    idx = [(a.find("input")==0) for a in args].index(True)
except ValueError:
    raise(ValueError,"Input image file must be specified using input=.")
else:
    inp_name = args.pop(idx).split("=")[1]
# Output image file
try:
    idx = [(a.find("output")==0) for a in args].index(True)
except ValueError:
    raise(ValueError,"Output image file must be specified using output=.")
else:
    out_name = args.pop(idx).split("=")[1]

img = dxtbx.load(inp_name)

from lunus.command_line.process import get_image_params, get_image_xvectors
experiment_params = get_image_params(img)
deck = experiment_params[0]
xvectors = get_image_xvectors(img)
raw_data = img.get_raw_data()
 # this is in photons, after applying the gain of the image (photon energy in keV)
data = img.get_imageset([inp_name])[0]
print(type(data[25]))
if not isinstance(data, tuple):
    data = data,
A = LunusDIFFIMAGE(len(data))
for panel_idx, panel in enumerate(data):
    A.set_image(panel_idx, panel)
    A.LunusSetparamsim(panel_idx,deck)
    A.set_xvectors(panel_idx,xvectors[panel_idx])
data_np = data[25].as_numpy_array()
hist=np.histogram(data_np,range=(-50,50),bins=100,density=False)
print(hist[0:100])
A.LunusAvgrim()
A.LunusSubrfim()
data_list=[]
for panel_idx, panel in enumerate(data):
    this_data = A.get_image_double(panel_idx)
    data_list.append(this_data)
data2 = tuple(data_list)
data2_np = data2[25].as_numpy_array()
hist=np.histogram(data2_np,range=(-50,50),bins=100,density=False)
print(hist[0:100])
writer = FullCBFWriter(inp_name)
cbf = writer.get_cbf_handle(header_only=True)
writer.add_data_to_cbf(cbf,data=data2)
writer.write_cbf(out_name, cbf=cbf)
