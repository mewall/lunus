# Written by David Wych, 4-1-2022
from pdbio import *
from sys import argv
from math import cos, sin, sqrt, radians
from copy import deepcopy


fi = argv[1]

# LOAD IN ENSEMBLE
ens = PDBFile(filename=fi)

a = ens.crystinfo.a
b = ens.crystinfo.b
c = ens.crystinfo.c
ca = cos(radians(ens.crystinfo.alpha))
cb = cos(radians(ens.crystinfo.beta))
cg = cos(radians(ens.crystinfo.gamma))
sg = sin(radians(ens.crystinfo.gamma))
v = sqrt(1 - ca*ca - cb*cb - cg*cg + 2*ca*cb*cg)


def convert_to_frac(atm):
    x = deepcopy(atm.x)
    y = deepcopy(atm.y)
    z = deepcopy(atm.z)
    fmat = [[1.0/a, -1.0*cg/(a*sg), ca*cg-cb/(v*a*sg)],
            [0.0,    1.0/(b*sg),    cb*cg-ca/(v*b*sg)],
            [0.0,    0.0,           sg/(c*v)]]
    atm.x = x*fmat[0][0] + y*fmat[0][1] + z*fmat[0][2]
    atm.y = x*fmat[1][0] + y*fmat[1][1] + z*fmat[1][2]
    atm.z = x*fmat[2][0] + y*fmat[2][1] + z*fmat[2][2]


def convert_to_cartesian(atm):
    x = deepcopy(atm.x)
    y = deepcopy(atm.y)
    z = deepcopy(atm.z)
    cmat = [[a,   b*cg, c*cb],
            [0.0, b*sg, (c/sg)*(ca-cb*cg)],
            [0.0, 0.0,  v*c/sg]]
    atm.x = x*cmat[0][0] + y*cmat[0][1] + z*cmat[0][2]
    atm.y = x*cmat[1][0] + y*cmat[1][1] + z*cmat[1][2]
    atm.z = x*cmat[2][0] + y*cmat[2][1] + z*cmat[2][2]


def UC(model, asu):
    """Takes in protein coordinates in array form and outputs coordinates
    transformed by P212121 symmetry operations
    """
    if asu == 0:
        pass
    if asu == 1:
        for atom in model.atoms():
            _x = deepcopy(atom.x)
            _y = deepcopy(atom.y)
            _z = deepcopy(atom.z)
            atom.x = -1.0*_x+0.5
            atom.y = -1.0*_y
            atom.z = _z+0.5

    if asu == 2:
        for atom in model.atoms():
            _x = deepcopy(atom.x)
            _y = deepcopy(atom.y)
            _z = deepcopy(atom.z)
            atom.x = _x+0.5
            atom.y = -1.0*_y+0.5
            atom.z = -1.0*_z

    if asu == 3:
        for atom in model.atoms():
            _x = deepcopy(atom.x)
            _y = deepcopy(atom.y)
            _z = deepcopy(atom.z)
            atom.x = -1.0*_x
            atom.y = _y+0.5
            atom.z = -1.0*_z+0.5

    return model


def TRN(model, axis):
    """Translates coordinates by a full unit cell along the selected direction
    """
    if axis == "x":
        for atom in model.atoms():
            _x = deepcopy(atom.x)
            atom.x = _x + 1.0

    elif axis == "y":
        for atom in model.atoms():
            _y = deepcopy(atom.y)
            atom.y = _y + 1.0

    elif axis == "z":
        for atom in model.atoms():
            _z = deepcopy(atom.z)
            atom.z = _z + 1.0

    return model


chainids = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
# WORKS ONLY FOR P212121 2x2x2 SUPERCELL
for model in ens.models.keys():
    # CONVERT TO FRACTIONAL COORDINATES
    for atom in ens.models[model].atoms():
        convert_to_frac(atom)
        atom.chainid = chainids[model-1]
    # OG UC
    if int((model-1)/4) == 0:
        UC(ens.models[model], (model-1) % 4)
    # UC +X
    if int((model-1)/4) == 1:
        TRN(UC(ens.models[model], (model-1) % 4), "x")
    # UC +Y
    if int((model-1)/4) == 2:
        TRN(UC(ens.models[model], (model-1) % 4), "y")
    # UC +Z
    if int((model-1)/4) == 3:
        TRN(UC(ens.models[model], (model-1) % 4), "z")
    # UC +XY
    if int((model-1)/4) == 4:
        TRN(TRN(UC(ens.models[model], (model-1) % 4), "x"), "y")
    # UC +XZ
    if int((model-1)/4) == 5:
        TRN(TRN(UC(ens.models[model], (model-1) % 4), "x"), "z")
    # UC +YZ
    if int((model-1)/4) == 6:
        TRN(TRN(UC(ens.models[model], (model-1) % 4), "y"), "z")
    # UC +XYZ
    if int((model-1)/4) == 7:
        TRN(TRN(TRN(UC(ens.models[model], (model-1) % 4), "x"), "y"), "z")
    # CONVERT BACK
    for atom in ens.models[model].atoms():
        convert_to_cartesian(atom)


# SAVE OUT THE FULL SUPERCELL
ens.crystinfo.a = ens.crystinfo.a * 2.0
ens.crystinfo.b = ens.crystinfo.b * 2.0
ens.crystinfo.c = ens.crystinfo.c * 2.0
ens.write("supercell.pdb", crystinfo=True, with_models=False)
