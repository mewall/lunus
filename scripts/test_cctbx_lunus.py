import lunus
import dxtbx
from dxtbx.format.FormatCBFMini import FormatCBFMini
from lunus import LunusDIFFIMAGE
import os

img = dxtbx.load("g150aich_274_6_1_00001.cbf")

detector = img.get_detector()
beam = img.get_beam()
gonio = img.get_goniometer()
scan = img.get_scan()

A = LunusDIFFIMAGE()
data = img.get_raw_data()
A.set_image(data)
deck = '''
#lunus input deck
punchim_xmin=1203
punchim_ymin=1250
punchim_xmax=2459
punchim_ymax=1314
windim_xmin=100
windim_ymin=100
windim_xmax=2362
windim_ymax=2426
thrshim_min=0
thrshim_max=50
modeim_bin_size=1
modeim_kernel_width=20

'''
A.LunusSetparamsim(deck)
A.LunusPunchim()
A.LunusWindim()
A.LunusThrshim()
A.LunusModeim()
print "Mode filter finished."
data2 = A.get_image();
#dxtbx.format.FormatCBFMini.FormatCBFMini.as_file(detector,beam,gonio,scan,data,path,header_convention="GENERIC_MINI",det_type="GENERIC")
FormatCBFMini.as_file(detector,beam,gonio,scan,data2,"g150aich_274_6_1_lunus_00001.cbf")
