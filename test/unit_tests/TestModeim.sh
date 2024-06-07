#!/bin/bash -c
export modeim_bin_size=1
export modeim_kernel_width=15
lunus.modeim ../data/TestImageSMV.img ../proc/TestModeim.img ${modeim_kernel_width} ${modeim_bin_size}
diff ../proc/TestModeim.img ../ref/TestModeimRef.img
