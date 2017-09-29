#!/bin/bash

# swaps the dimension sizes in an Eiger CBF to prep for conversion to .img
# this does NOT preserve the original file! best to use after adding the header, while the original no_header version still exists somewhere else.

#Veronica Pillar 2-26-14

for i in "$@"; do
	sed 's/Fastest-Dimension: 1030/Fastest-Dimension: 1065/' <$i >temp
	sed 's/Second-Dimension: 1065/Second-Dimension: 1030/' <temp >$i
done
