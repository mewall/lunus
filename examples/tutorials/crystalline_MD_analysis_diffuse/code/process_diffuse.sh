lunus.hkl2lat $1 diffuse_$2.lat template.lat
lunus.avgrlt diffuse_$2.lat isotropic_$2.rf $(cat supercell_cell.txt)
lunus.binasc 2 < isotropic_$2.rf > isotropic_$2.dat
