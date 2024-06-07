export windim_xmax=984
export windim_xmin=40
export windim_ymax=984
export windim_ymin=40
lunus.windim ../data/TestImageSMV.img ${windim_xmin} ${windim_xmax} ${windim_ymin} ${windim_ymax} ../proc/TestWindim.img
diff ../proc/TestWindim.img ../ref/TestWindimRef.img
