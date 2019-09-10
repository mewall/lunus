import numpy as np
import scipy.optimize
import subprocess, shlex
import sys
from iotbx.pdb import hierarchy

fiter = 0

def calc_corr_llm(x):
    global fiter
    fiter = fiter + 1
    if (x[0]>0 and x[1]>0):

        command = 'llmlt Icalc_sym.lat tmp.lat {0} {1} {2}'.format(cell_str,x[0],x[1])
        call_params = shlex.split(command)
        subprocess.call(call_params)

        command = 'symlt tmp.lat tmp_sym.lat {0}'.format(symop)
        call_params = shlex.split(command)
        subprocess.call(call_params)

        command = 'anisolt tmp_sym.lat tmp_sym_aniso.lat {0}'.format(cell_str)
        print command
        call_params = shlex.split(command)
        subprocess.call(call_params)

        f = open("this_correl.txt","w")
        command = 'corrlt tmp_sym_aniso.lat {0}'.format(data_file)
        print command
        call_params = shlex.split(command)
        subprocess.call(call_params,stdout=f)
        f.close()

        f = open("this_correl.txt")
        clist = [0.0,f.read().splitlines()[-1]]
        f.close()
    else:
        clist = [0,0]
    print "Function iteration = ",fiter,", (gamma,sigma) = (",x[0],",",x[1],"), overall correlation = ",clist[0],", anisotropic correlation (target) = ",clist[1]
    sys.stdout.flush()
    return -float(clist[1])

def calc_corr_llm2(x):
    global fiter
    fiter = fiter + 1
    if (x[0]>0 and x[1]>0 and x[2]>0):

        command = 'llmlt Icalc_sym.lat tmp.lat {0} {1} {2} {3}'.format(cell_str,x[0],x[1],x[2])
        call_params = shlex.split(command)
        subprocess.call(call_params)

        command = 'symlt tmp.lat tmp_sym.lat {0}'.format(symop)
        call_params = shlex.split(command)
        subprocess.call(call_params)

        command = 'anisolt tmp_sym.lat tmp_sym_aniso.lat {0}'.format(cell_str)
        print command
        call_params = shlex.split(command)
        subprocess.call(call_params)

        f = open("this_correl.txt","w")
        command = 'corrlt tmp_sym_aniso.lat {0}'.format(data_file)
        print command
        call_params = shlex.split(command)
        subprocess.call(call_params,stdout=f)
        f.close()

        f = open("this_correl.txt")
        clist = [0.0,f.read().splitlines()[-1]]
        f.close()
    else:
        clist = [0,0]
    print "Function iteration = ",fiter,", (gamma,sigma1,sigma2) = (",x[0],",",x[1],",",x[2],"), overall correlation = ",clist[0],", anisotropic correlation (target) = ",clist[1]
    sys.stdout.flush()
    return -float(clist[1])

def calc_corr_rbt(x):
    global fiter
    fiter = fiter + 1
    if (x[0]>0):

        command = 'rbtlt Icalc_sym.lat tmp.lat {0} {1}'.format(cell_str,x[0])
        call_params = shlex.split(command)
        subprocess.call(call_params)

        command = 'symlt tmp.lat tmp_sym.lat {0}'.format(symop)
        call_params = shlex.split(command)
        subprocess.call(call_params)

        command = 'anisolt tmp_sym.lat tmp_sym_aniso.lat {0}'.format(cell_str)
        print command
        call_params = shlex.split(command)
        subprocess.call(call_params)

        f = open("this_correl.txt","w")
        command = 'corrlt tmp_sym_aniso.lat {0}'.format(data_file)
        print command
        call_params = shlex.split(command)
        subprocess.call(call_params,stdout=f)
        f.close()

        f = open("this_correl.txt")
        clist = [0.0,f.read().splitlines()[-1]]
        f.close()
    else:
        clist = [0,0]
    print "Function iteration = ",fiter,", (sigma) = (",x[0],"), overall correlation = ",clist[0],", anisotropic correlation (target) = ",clist[1]
    sys.stdout.flush()
    return -float(clist[1])

def calc_corr_rbt2(x):
    global fiter
    fiter = fiter + 1
    if (x[0]>0 and x[1]>0):

        command = 'rbtlt Icalc_sym.lat tmp.lat {0} {1} {2}'.format(cell_str,x[0],x[1])
        call_params = shlex.split(command)
        subprocess.call(call_params)

        command = 'symlt tmp.lat tmp_sym.lat {0}'.format(symop)
        call_params = shlex.split(command)
        subprocess.call(call_params)

        command = 'anisolt tmp_sym.lat tmp_sym_aniso.lat {0}'.format(cell_str)
        print command
        call_params = shlex.split(command)
        subprocess.call(call_params)

        f = open("this_correl.txt","w")
        command = 'corrlt tmp_sym_aniso.lat {0}'.format(data_file)
        print command
        call_params = shlex.split(command)
        subprocess.call(call_params,stdout=f)
        f.close()

        f = open("this_correl.txt")
        clist = [0.0,f.read().splitlines()[-1]]
        f.close()
    else:
        clist = [0,0]
    print "Function iteration = ",fiter,", (sigma1,sigma2) = (",x[0],",",x[1],"), overall correlation = ",clist[0],", anisotropic correlation (target) = ",clist[1]
    sys.stdout.flush()
    return -float(clist[1])

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

# Input data file

    try:
        idx = [a.find("data")==0 for a in args].index(True)
    except ValueError:
        data_file = "data.lat"
    else:
        data_file = args.pop(idx).split("=")[1]

# Input model type

    try:
        idx = [a.find("model")==0 for a in args].index(True)
    except ValueError:
        model_type = "llm"
    else:
        model_type = args.pop(idx).split("=")[1]

# Input symmetry operation for symlt

    try:
        idx = [a.find("symop")==0 for a in args].index(True)
    except ValueError:
        symop = 0
    else:
        symop = args.pop(idx).split("=")[1]

# Compute Icalc from the .pdb input

    pdb_in = hierarchy.input(file_name=pdb_file)

    xrs = pdb_in.input.xray_structure_simple()

    if (bfacs == "zero"):
        xrs.convert_to_isotropic()
        xrs.set_b_iso(0.0)

    if (bfacs == "iso"):
        xrs.convert_to_isotropic()

#    scat = xrs.scatterers()
#    print "Length of scatterers array = ",len(scat)
    xrs.set_occupancies(1.0)
    fcalc = xrs.structure_factors(d_min=1.0).f_calc()
    f_000 = xrs.f_000()
#    if (f_000*f_000 == 0.0):
#        print "f_000*f_000 is zero"
    fc_square = fcalc.as_intensity_array()
    fc_square_p1 = fc_square.expand_to_p1()

    f=open("tmp.hkl",'w')
    print >>f, "%4d %4d %4d   %10.2f" %((0,0,0)+tuple((f_000*f_000,)))
    for hkl,intensity in fc_square_p1:
        print >>f, "%4d %4d %4d   %10.2f" %(hkl+tuple((intensity,)))
    f.close()

# Make the Icalc_sym.lat file needed for the llm modeling

    command = 'hkl2lat tmp.hkl tmp.lat {0}'.format(data_file)
    call_params = shlex.split(command)
    subprocess.call(call_params)

    command = 'symlt tmp.lat Icalc_sym.lat {0}'.format(symop)
    call_params = shlex.split(command)
    subprocess.call(call_params)

    cell = xrs.unit_cell().parameters()

    cell_str = "{0},{1},{2},{3},{4},{5}".format(cell[0],cell[1],cell[2],cell[3],cell[4],cell[5])

# Refine the llm model

    if (model_type == "llm"):

        x0 = [5.,0.36]
        res = scipy.optimize.minimize(calc_corr_llm,x0,method='Powell',jac=None,options={'disp': True,'maxiter': 10000})
        gamma = res.x[0]
        sigma = res.x[1]
        print "Refined gamma = ",gamma,", sigma = ",sigma

# Refine the llm 2 model

    if (model_type == "llm2"):

        x0 = [5.,0.36,.36]
        res = scipy.optimize.minimize(calc_corr_llm2,x0,method='Powell',jac=None,options={'disp': True,'maxiter': 10000})
        gamma = res.x[0]
        sigma1 = res.x[1]
        sigma2 = res.x[2]
        print "Refined gamma = ",gamma,", sigma1 = ",sigma1,", sigma2 = ",sigma2

    if (model_type == "rbt"):
        x0 = [0.36]
        res = scipy.optimize.minimize(calc_corr_rbt,x0,method='Powell',jac=None,options={'disp': True,'maxiter': 10000})
        sigma = res.x
        print "Refined sigma = ",sigma

    if (model_type == "rbt2"):
        x0 = [0.36,0.36]
        res = scipy.optimize.minimize(calc_corr_rbt2,x0,method='Powell',jac=None,options={'disp': True,'maxiter': 10000})
        sigma1 = res.x[0]
        sigma2 = res.x[1]
        print "Refined sigma1 = ",sigma1,", sigma2 = ",sigma2
