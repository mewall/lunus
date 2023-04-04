#!/bin/bash

# arguments:
# 1: diffuse .lat file

IN=$1

lunus.avgrlt $IN ${IN%.lat}_isotropic.rf $(cat cell.txt)
lunus.binasc 2 < ${IN%.lat}_isotropic.rf > ${IN%.lat}_isotropic.dat
