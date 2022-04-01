# Written by David Wych, 4-1-2022
import math


class Atom:
    """Class containing ATOM and HETATM information
    """
    def __init__(self, line):
        if line[0:6] not in ["ATOM  ", "HETATM"]:
            print("ERROR: lines passed in to Atom must have ATOM or HETATM records")
        else:
            self.type = line[0:6].strip()        # ATOM or HETATM
            self.index = int(line[6:11])         # INDEX
            self.atomtype = line[12:16].strip()  # ATOM TYPE
            self.altid = line[16]                # MULTICONF ALT ID
            self.resname = line[17:20]           # RESIDUES NAME
            self.chainid = line[21]              # CHAIN ID
            self.resid = int(line[22:27])        # RESIDUE NUMBER
            self.x = float(line[30:38])          # X COORD
            self.y = float(line[38:46])          # Y COORD
            self.z = float(line[46:54])          # Z COORD
            self.occ = float(line[54:60])        # OCCUPANCY
            self.bfac = float(line[60:66])       # B-FACTOR
            self.segid = line[72:76]             # SEQUENCE ID
            self.element = line[76:78]           # ATOMIC ELEMENT

    def __repr__(self):
        return "{} {} in {}{}".format(self.atomtype, self.index, self.resname, self.resid)


class Residue:
    """Class containing ATOM objects in a particular residue
    """
    def __init__(self, resid=None, resname=None, chainid=None):
        if resid is None:
            print("please give the residue a resid")
            pass
        if resname is None:
            print("please give the residue a resname")
            pass
        if chainid is None:
            print("please give the residue a chainid")
            pass
        else:
            self.chainid = chainid
            self.resid = resid
            self.resname = resname
            self._atoms = []

    def add_atom(self, atom):
        self._atoms.append(atom)

    def atoms(self):
        return self._atoms

    def __repr__(self):
        return "{}{}".format(self.resname, self.resid)


class Chain:
    """Class containing the Residue objects in a chain
    """
    def __init__(self, chainid=None, modelid=None):
        if chainid is None:
            print("please give the chain a chainid")
            pass
        else:
            if modelid is not None:
                self.modelid = modelid
            self.chainid = chainid
            self._residues = []

    def add_residue(self, resid=None, resname=None):
        self._residues.append(Residue(resid, resname, chainid=self.chainid))

    def residues(self):
        return self._residues

    def atoms(self):
        _atoms = [residue.atoms() for residue in self.residues()]
        return [atom for atoms in _atoms for atom in atoms]

    def __repr__(self):
        return "chain {}".format(self.chainid)


class Model:
    """Class containing the Chain objects in a model
    """
    def __init__(self, modelid=None):
        self.modelid = modelid
        self._chains = []

    def add_chain(self, chainid='A'):
        self._chains.append(Chain(chainid=chainid, modelid=self.modelid))

    def chains(self):
        return self._chains

    def residues(self):
        _residues = [chain.residues() for chain in self.chains()]
        return [res for residue in _residues for res in residue]

    def atoms(self):
        _atoms = [res.atoms() for res in self.residues()]
        return [atom for atoms in _atoms for atom in atoms]


class CrystInfo:
    """Class containing all the crystallographic information
    """
    def __init__(self, line):
        self.a = float(line[6:15])
        self.b = float(line[15:24])
        self.c = float(line[24:33])
        self.alpha = float(line[33:40])
        self.beta = float(line[40:47])
        self.gamma = float(line[47:54])
        self.sgroup = line[55:66]
        self.zval = line[66:70]
        # crystal lattice vectors
        cg = math.cos(math.radians(self.gamma))
        sg = math.sin(math.radians(self.gamma))
        cb = math.cos(math.radians(self.beta))
        ca = math.cos(math.radians(self.alpha))
        c1 = (ca - cb * cg) / sg
        self.avec = [self.a, 0.0, 0.0]
        self.bvec = [self.b*cg, self.b*sg, 0.0]
        self.cvec = [self.c*cb, self.c*c1, self.c*math.sqrt(1 - cb*cb - c1*c1)]
        for vec in [self.avec, self.bvec, self.cvec]:
            for i in range(3):
                if math.isclose(vec[i], 0.0, abs_tol=0.0001):
                    vec[i] = 0.0

        # normalized lattice vectors
        magA = self.avec[0]
        magB = math.sqrt(self.bvec[0]*self.bvec[0] + self.bvec[1]*self.bvec[1])
        magC = math.sqrt(self.cvec[0]*self.cvec[0] + self.cvec[1]*self.cvec[1] + self.cvec[2]*self.cvec[2])
        self.avec_norm = [self.avec[0] / magA, 0.0, 0.0]
        self.bvec_norm = [self.bvec[0] / magB, self.bvec[1] / magB, 0.0]
        self.cvec_norm = [self.cvec[0] / magC, self.cvec[1] / magC, self.cvec[2] / magC]
                

class PDBFile:
    """Class containing the contents of a PDB file
    """
    def __init__(self, filename=None, crystinfo=None, with_models=False):
        # If you initialize an empty PDBFile object...
        if filename is None:
            self.filename = None
            self.crystinfo = crystinfo
            self.header = []
            # If you want a multi-model PDBFile...
            if with_models:
                self.models = dict()
                self.models[1] = Model(modelid=1)
            # Otherwise...
            else:
                self.models = None
                self.chains = [Chain(chainid="A")]
        # Otherwise, it's going to expect a filename:
        else:
            self.filename = filename
            with open(self.filename, "r") as f:
                lines = f.readlines()
                self.system_lines = []
                self.header = []
                for line in lines:
                    # Store all the header information for output later
                    if line[:6] not in ["ATOM  ", "HETATM", "ANISOU", "TER   ", "END   ", "MASTER", "CRYST1"]:
                        self.header.append(line)
                    if line[:6] == "CRYST1":
                        self.crystinfo = CrystInfo(line)
                    if line[:5] == "MODEL":
                        self.system_lines.append(line)
                    if line[:6] in ["ATOM  ", "HETATM"]:
                        self.system_lines.append(line)
                    if line[:3] in ["TER", "END"]:
                        self.system_lines.append(line)

            # look for models
            models = []
            for line in self.system_lines:
                if line[:5] == "MODEL":
                    models.append(int(line.split()[-1]))

            # if there is model information
            if len(models) > 0:
                self.models = dict()
                for model in models:
                    self.models[model] = Model(modelid=model)
                for line in self.system_lines:
                    if line[:5] == "MODEL":
                        modelid = int(line.split()[-1])
                    else:
                        self._add_from_pdb(line=line, modelid=modelid)

            # otherwise, add a single model
            else:
                self.models = dict()
                self.models[1] = Model(modelid=1)
                for line in self.system_lines:
                    self._add_from_pdb(line=line, modelid=1)

    def propagate_atom(self, atom=None, modelid=None):
        if atom is None or modelid is None:
            print("Need to supply and atom and a modelid")
        # If the chain isn't already in the model...
        if atom.chainid not in [chain.chainid for chain in self.models[modelid].chains()]:
            # Add it.
            self.models[modelid].add_chain(chainid=atom.chainid)
            # Then add the residue to the chain...
            for chain in self.models[modelid].chains():
                if atom.chainid == chain.chainid:
                    chain.add_residue(resid=atom.resid, resname=atom.resname)
                    # And add the atom to the residue.
                    for residue in chain.residues():
                        if atom.resid == residue.resid and atom.resname == residue.resname:
                            residue.add_atom(atom)
        # If the chain is already in the model
        else:
            # Get the right chain...
            for chain in self.models[modelid].chains():
                if atom.chainid == chain.chainid:
                    # If the residue is not already in the chain, add it
                    if "{}{}".format(atom.resname, atom.resid) not in [repr(res) for res in chain.residues()]:
                        chain.add_residue(resid=atom.resid, resname=atom.resname)
                        for residue in chain.residues():
                            if atom.resid == residue.resid and atom.resname == residue.resname:
                                residue.add_atom(atom)
                    # Otherwise just add the atom to the residue
                    else:
                        for residue in chain.residues():
                            if atom.resid == residue.resid and atom.resname == residue.resname:
                                residue.add_atom(atom)

    def _add_from_pdb(self, line=None, modelid=1):
        if line[:6] in ["ATOM  ", "HETATM"]:
            atom = Atom(line)
            self.propagate_atom(atom, modelid)

    def chains(self, all_models=False):
        if len(self.models.keys()) > 1 and all_models is False:
            print("ERROR: Plase select a model")
        elif len(self.models.keys()) > 1 and all_models is True:
            model_chains = [self.models[i].chains() for i in self.models.keys()]
            return [chain for chains in model_chains for chain in chains]
        else:
            return self.models[1].chains()

    def residues(self, all_models=False):
        if len(self.models.keys()) > 1 and all_models is False:
            print("ERROR: Please select a model")
        elif len(self.models.keys()) > 1 and all_models is True:
            model_chains = [self.models[i].chains() for i in self.models.keys()]
            _chains = [chain for chains in model_chains for chain in chains]
            _residues = [chain.residues() for chain in _chains]
            return [residue for residues in _residues for residue in residues]
        else:
            _residues = [chain.residues() for chain in self.models[1].chains()]
            return [residue for residues in _residues for residue in residues]

    def add_model(self):
        if len(self.models.keys()) == 0:
            self.models = dict()
            self.models[1] = Model(modelid=1)
        else:
            max_model_id = max(list(self.models.keys()))
            self.models[max_model_id+1] = Model(max_model_id+1)

    def write(self, ofilename, crystinfo=False, header=False, with_models=True):
        with open(ofilename, 'w') as of:
            if header:
                for line in self.header:
                    of.write(line)
            if crystinfo:
                of.write("CRYST1{:>9}{:>9}{:>9}{:>7}{:>7}{:>7} {:<11}".format("{:.3f}".format(self.crystinfo.a),
                                                                              "{:.3f}".format(self.crystinfo.b),
                                                                              "{:.3f}".format(self.crystinfo.c),
                                                                              "{:.2f}".format(self.crystinfo.alpha),
                                                                              "{:.2f}".format(self.crystinfo.beta),
                                                                              "{:.2f}".format(self.crystinfo.gamma),
                                                                              self.crystinfo.sgroup))
            for model in self.models.keys():
                if with_models:
                    of.write("MODEL       {:>2}\n".format(model))

                for chain in self.models[model].chains():
                    for atom in chain.atoms():
                        if len(atom.atomtype) == 1:
                            atom.atomtype = " " + atom.atomtype + "  "
                        if len(atom.atomtype) == 2:
                            atom.atomtype = " " + atom.atomtype + " "
                        if len(atom.atomtype) == 3:
                            atom.atomtype = " " + atom.atomtype

                        of.write("ATOM  {:>5} {:<4}{}{:<3} {}{:>4}    {:>8}{:>8}{:>8}{:>6}{:>6}          {:>2}  \n".format(atom.index,
                                                                                                                           atom.atomtype,
                                                                                                                           atom.altid,
                                                                                                                           atom.resname,
                                                                                                                           atom.chainid,
                                                                                                                           atom.resid,
                                                                                                                           "{:.3f}".format(atom.x),
                                                                                                                           "{:.3f}".format(atom.y),
                                                                                                                           "{:.3f}".format(atom.z),
                                                                                                                           "{:.2f}".format(atom.occ),
                                                                                                                           "{:.2f}".format(atom.bfac),
                                                                                                                           atom.element))
                    of.write("TER\n")
                if with_models:
                    of.write("ENDMDL\n")
