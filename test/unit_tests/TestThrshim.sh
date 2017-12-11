export thrshim_max=10000
export thrshim_min=1
thrshim ../data/TestImageSMV.img ${thrshim_min} ${thrshim_max} ../proc/TestThrshim.img
diff ../proc/TestThrshim.img ../ref/TestThrshimRef.img 
