from iotbx.pdb import hierarchy
import subprocess,shlex

if __name__=="__main__":
    import sys

    args = sys.argv[1:]

# Input .pdb file

    try:
        idx = [a.find("pdb")==0 for a in args].index(True)
    except ValueError:
        pdb_file = "input.pdb"
    else:
        pdb_file = args.pop(idx).split("=")[1]

# Resolution

    try:
        idx = [(a.find("resolution")==0 or a.find("d_min")==0) for a in args].index(True)
    except ValueError:
        d_min = 1.0
    else:
        d_min = float(args.pop(idx).split("=")[1])

# B factor treatment

    try:
        idx = [(a.find("bfacs")==0 or a.find("adps")==0) for a in args].index(True)
    except ValueError:
        bfacs = "aniso"
    else:
        bfacs = args.pop(idx).split("=")[1]

# Template .lat file

    try:
        idx = [(a.find("template")==0 or a.find("data")==0) for a in args].index(True)
    except ValueError:
        template = "data.lat"
    else:
        template = args.pop(idx).split("=")[1]

# Unit cell file

    try:
        idx = [(a.find("cell")==0 or a.find("cell_file")==0) for a in args].index(True)
    except ValueError:
        cell_file = "cell"
    else:
        cell_file = args.pop(idx).split("=")[1]

    pdb_in = hierarchy.input(file_name=pdb_file)

    xrs = pdb_in.input.xray_structure_simple()

    if (bfacs == "zero"):
        xrs.convert_to_isotropic()
        xrs.set_b_iso(0.0)

    if (bfacs == "iso"):
        xrs.convert_to_isotropic()

    fcalc = xrs.structure_factors(d_min=1.0).f_calc()
    fc_square = fcalc.as_intensity_array()
    fc_square_p1 = fc_square.expand_to_p1()

    f=open("tmp.hkl",'w')
    for hkl,intensity in fc_square_p1:
        print >>f, "%4d %4d %4d   %10.2f" %(hkl+tuple((intensity,)))
    f.close()

# Make the Icalc_sym.lat file needed for the llm modeling

    command = 'hkl2lat tmp.hkl tmp.lat {0}'.format(template)
    call_params = shlex.split(command)
    subprocess.call(call_params)

    command = 'symlt tmp.lat Icalc_sym.lat 0'
    call_params = shlex.split(command)
    subprocess.call(call_params)

# Output the unit cell

    cell = xrs.unit_cell().parameters()

    f=open(cell_file,'w')
    print >>f,"{0},{1},{2},{3},{4},{5}".format(cell[0],cell[1],cell[2],cell[3],cell[4],cell[5])
    f.close()
    
