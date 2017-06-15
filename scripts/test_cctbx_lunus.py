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
data = img.get_raw_data()

print data

#print detector

#print img

A = LunusDIFFIMAGE()
A.set_raw_data(data)
print A.get_image_data_type_size()
A.set_punchim_lower(1203,1250)
A.set_punchim_upper(2459,1314)
A.LunusPunchim()
A.set_windim_lower(100,100)
A.set_windim_upper(2362,2426)
A.LunusWindim()
A.set_thresholds(0,50)
A.LunusThrshim()
A.set_modeim_width(20)
A.LunusModeim()
print "Mode filter finished."
data2 = A.get_lunus_data();
#dxtbx.format.FormatCBFMini.FormatCBFMini.as_file(detector,beam,gonio,scan,data,path,header_convention="GENERIC_MINI",det_type="GENERIC")
FormatCBFMini.as_file(detector,beam,gonio,scan,data2,"g150aich_274_6_1_lunus_00001.cbf")
