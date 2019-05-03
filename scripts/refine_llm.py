import numpy as np
import scipy.optimize
import subprocess, shlex
import sys

fiter = 0

def calc_corr(x):
    global fiter
    fiter = fiter + 1
    if (x[0]>0 and x[1]>0):

        command = 'llmlt Icalc_sym.lat tmp.lat {0} {1} {2}'.format(cell,x[0],x[1])
        call_params = shlex.split(command)
        subprocess.call(call_params)

        command = 'symlt tmp.lat tmp_sym.lat {0}'.format(symop)
        call_params = shlex.split(command)
        subprocess.call(call_params)

        command = 'anisolt tmp_sym.lat tmp_sym_aniso.lat {0}'.format(cell)
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

if __name__=="__main__":

    import sys

    args = sys.argv[1:]

# Input data file

    try:
        idx = [a.find("data")==0 for a in args].index(True)
    except ValueError:
        data_file = "data.lat"
    else:
        data_file = args.pop(idx).split("=")[1]

# Input unit cell file

    try:
        idx = [a.find("cell")==0 for a in args].index(True)
    except ValueError:
        cell_file = "cell"
    else:
        cell_file = args.pop(idx).split("=")[1]


# Input symmetry operation for symlt

    try:
        idx = [a.find("symop")==0 for a in args].index(True)
    except ValueError:
        symop = 0
    else:
        symop = args.pop(idx).split("=")[1]

    f = open(cell_file,"r")
    cell = f.read()
    f.close()

    x0 = [5.,0.36]
    res = scipy.optimize.minimize(calc_corr,x0,method='Powell',jac=None,options={'disp': True,'maxiter': 10000})
    gamma = res.x[0]
    sigma = res.x[1]
    print "Refined gamma = ",gamma,", sigma = ",sigma
