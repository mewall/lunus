# Reads an INTEGRATE.LP file (from XDS) and extracts the orientation info
# necessary for genlat_xds_parallel.py into a sequence of XDS_ASCII.head files.
# Veronica Pillar, 12-22-15
# Inspired by Mike Wall's code

# input on command line: 
# input.fname=[INTEGRATE.LP name]
# output.template=[output template, e.g. XDS_ASCII_1deg_ will give you files
# named XDS_ASCII_1deg_001.head, XDS_ASCII_1deg_002.head, etc.]


if __name__=="__main__":
    import sys
    import re

    print "hi"

    args = sys.argv[1:] 

    for a in args:
        print a

    # Read command line arguments

    try: 
        ifnameidx = [a.find("input.fname")==0 for a in args].index(True)
    except ValueError:
        ifname = "INTEGRATE.LP"
    else:
        ifname = args.pop(ifnameidx).split("=")[1]
    try:
        oftemplateidx = [a.find("output.template")==0 for a in args].index(True)
    except ValueError:
        oftemplate = "XDS_ASCII_"
    else:
        oftemplate = args.pop(oftemplateidx).split("=")[1]

    import os

    # read input file
    f = open(ifname,"r")
    lines = []
    for line in f:
        if 'REFERENCE PROFILES AS FUNCTION OF DETECTOR POSITION' in line:
            break
        else:
            lines.append(line)
    f.close()

    ofnum = 1 #outfile counter--increment every time you close one

    for line in lines:
        left,sep,right = line.partition('PROCESSING OF IMAGES')
        if sep: # then we are at the head of an image group
            # now we're going to figure out what degree this group is
            # assuming 0.1 deg images as XDS input
            left2,sep2,right2 = right.partition('...')
            deg = int(round((int(left2) + int(right2) + 9)/20))
            #now open the file
            ofname = oftemplate+str(deg).zfill(3)+'.head'
            of = open(ofname,"w")
            print >>of,"!WAVELENGTH=",wavelength
            continue
        left,sep,right = line.partition('DETECTOR ORIGIN (PIXELS) AT')
        if sep:
            #left2,sep2,right2 = right.partition('  ') #this doesn't work--fix later
            print >>of,"!ORGX=",right.split()[0],"  ORGY=",right.split()[1]
            continue
        left,sep,right = line.partition('DISTANCE (mm)')
        if sep:
            print >>of,"!DETECTOR_DISTANCE=",right
            # this is the last one in .LP order, so we close up and increment
            of.close()
            ofnum = ofnum+1
            continue
        left,sep,right = line.partition('UNIT CELL A-AXIS')
        if sep:
            print >>of,"!UNIT_CELL_A-AXIS=",right
            continue
        left,sep,right = line.partition('UNIT CELL B-AXIS')
        if sep:
            print >>of,"!UNIT_CELL_B-AXIS=",right
            continue
        left,sep,right = line.partition('UNIT CELL C-AXIS')
        if sep:
            print >>of,"!UNIT_CELL_C-AXIS=",right
            continue
        left,sep,right = line.partition('ROTATION AXIS')
        if sep:
            print >>of,"!ROTATION_AXIS=",right
            continue
        left,sep,right = line.partition('BEAM COORDINATES (REC. ANGSTROEM)')
        if sep: 
            print >>of,"!INCIDENT_BEAM_DIRECTION=",right
            continue
        left,sep,right = line.partition('X-RAY_WAVELENGTH=')
        if sep: # this should only happen once, at the beginning
            left2,sep2,right2 = right.partition('ANGSTROM')
            wavelength = left2
            continue

