# Written by David Wych, 4-1-2022
from pdbio import *
from sys import argv
from math import cos, sin, sqrt, radians
from copy import deepcopy


fi = argv[1]

# LOAD IN ENSEMBLE
sup = PDBFile(filename=fi)

a = sup.crystinfo.a/2
b = sup.crystinfo.b/2
c = sup.crystinfo.c/2
ca = cos(radians(sup.crystinfo.alpha))
cb = cos(radians(sup.crystinfo.beta))
cg = cos(radians(sup.crystinfo.gamma))
sg = sin(radians(sup.crystinfo.gamma))
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
            atom.x = -1.0 * (_x - 0.5)
            atom.y = -1.0 * _y
            atom.z = _z - 0.5

    if asu == 2:
        for atom in model.atoms():
            _x = deepcopy(atom.x)
            _y = deepcopy(atom.y)
            _z = deepcopy(atom.z)
            atom.x = _x - 0.5
            atom.y = -1.0 * (_y - 0.5)
            atom.z = -1.0 * _z

    if asu == 3:
        for atom in model.atoms():
            _x = deepcopy(atom.x)
            _y = deepcopy(atom.y)
            _z = deepcopy(atom.z)
            atom.x = -1.0 * _x
            atom.y = _y - 0.5
            atom.z = -1.0 * (_z - 0.5)

    return model


def TRN(model, axis):
    """Translates coordinates by a full unit cell along the selected direction
    """
    if axis == "x":
        for atom in model.atoms():
            _x = deepcopy(atom.x)
            atom.x = _x - 1.0

    elif axis == "y":
        for atom in model.atoms():
            _y = deepcopy(atom.y)
            atom.y = _y - 1.0

    elif axis == "z":
        for atom in model.atoms():
            _z = deepcopy(atom.z)
            atom.z = _z - 1.0

    return model

ens = PDBFile(filename=None, crystinfo=sup.crystinfo, with_models=True)
for _ in range(31):
    ens.add_model()

print(ens.models.keys())

chainids = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
for chain in sup.models[1].chains():
    model = chainids.index(chain.chainid) + 1
    print(model)
    # CONVERT TO FRACTIONAL COORDINATES
    for atom in chain.atoms():
        convert_to_frac(atom)
        ens.propagate_atom(atom, modelid=model)
    print(len(ens.models[model].atoms()))
    # OG UC
    if int((model-1)/4) == 0:
        UC(ens.models[model], (model-1) % 4)
    # UC +X
    if int((model-1)/4) == 1:
        UC(TRN(ens.models[model], "x"), (model-1) % 4)
    # UC +Y
    if int((model-1)/4) == 2:
        UC(TRN(ens.models[model], "y"), (model-1) % 4)
    # UC +Z
    if int((model-1)/4) == 3:
        UC(TRN(ens.models[model], "z"), (model-1) % 4)
    # UC +XY
    if int((model-1)/4) == 4:
        UC(TRN(TRN(ens.models[model], "x"), "y"), (model-1) % 4)
    # UC +XZ
    if int((model-1)/4) == 5:
        UC(TRN(TRN(ens.models[model], "x"), "z"), (model-1) % 4)
    # UC +YZ
    if int((model-1)/4) == 6:
        UC(TRN(TRN(ens.models[model], "y"), "z"), (model-1) % 4)
    # UC +XYZ
    if int((model-1)/4) == 7:
        UC(TRN(TRN(TRN(ens.models[model], "x"), "y"), "z"), (model-1) % 4)
    # CONVERT BACK
    for atom in ens.models[model].atoms():
        convert_to_cartesian(atom)
    print(len(ens.models[model].atoms()))

# SAVE OUT THE FULL SUPERCELL
ens.crystinfo.a = ens.crystinfo.a / 2.0
ens.crystinfo.b = ens.crystinfo.b / 2.0
ens.crystinfo.c = ens.crystinfo.c / 2.0
ens.write("reverse_prop_ensemble.pdb", crystinfo=True, with_models=True)
