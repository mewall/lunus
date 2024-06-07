export polarim_dist=57.42
export polarim_offset=0.0
export polarim_polarization=0.93
export correction_factor_scale=1.0
lunus.cfim ../data/TestImageSMV.img ../proc/TestCfim.imf ${polarim_dist} ${polarim_offset} ${polarim_polarization} ${correction_factor_scale}
diff ../proc/TestCfim.imf ../ref/TestCfimRef.imf
