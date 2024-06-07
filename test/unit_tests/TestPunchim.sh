export punchim_xmax=556 
export punchim_xmin=510 
export punchim_ymax=517
export punchim_ymin=473
lunus.punchim ../data/TestImageSMV.img ${punchim_xmin} ${punchim_xmax} ${punchim_ymin} ${punchim_ymax} ../proc/TestPunchim.img
diff ../proc/TestPunchim.img ../ref/TestPunchimRef.img
