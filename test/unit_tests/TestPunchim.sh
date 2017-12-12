export punchim_xmax=556 
export punchim_xmin=510 
export punchim_ymax=473 
export punchim_ymin=517 
punchim ../data/TestImageSMV.img ${punchim_xmin} ${punchim_xmax} ${punchim_ymin} ${punchim_ymax} ../proc/TestPunchim.img
diff ../proc/TestPunchim.img ../ref/TestPunchimRef.img
