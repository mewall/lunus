# calc_isotropic.sh

lunus.makelt template.lat $(cat supercell_cell.txt) 1.6
lunus.hkl2lat diffuse_supercell.hkl diffuse.lat template.lat
lunus.avgrlt diffuse.lat isotropic.rf $(cat supercell_cell.txt)
lunus.binasc 2 < isotropic.rf > isotropic.dat
