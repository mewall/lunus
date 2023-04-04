import iotbx.pdb
import cctbx
import scitbx.matrix
import itertools
import string
import numpy as np
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-xrs", "--input_xrs", help="xray structure pdb file")
parser.add_argument("-s",   "--input_sc",  help="supercell pdb file")
parser.add_argument("-o",   "--output",    help="output pdb file")
args = parser.parse_args()

xrs_pdb = args.input_xrs
sc_pdb  = args.input_sc
out_pdb = args.output

def reverse_propagate_supercell(supercell_pdb_filename, xrs_pdb_filename):
    '''

    Parameters
    ----------
    supercell_pdb_filename : str
        filename of supercell pdb file

    xrs_pdb_filename : str
        filename of x-ray structure file

    Returns
    -------
    ens : iotbx_pdb_hierarchy_ext.root
        cctbx hierarchy object for the ensemble
    '''
    
    def _get_unit_cell_vectors(xray_structure):
        A, B, C, alpha, beta, gamma = xray_structure.crystal_symmetry().unit_cell().parameters()
        cg = np.cos(np.radians(gamma))
        sg = np.sin(np.radians(gamma))
        cb = np.cos(np.radians(beta))
        ca = np.cos(np.radians(alpha))
        c1 = (ca - cb * cg) / sg

        avec = np.array([A, 0.0, 0.0])
        bvec = np.array([B * cg, B * sg, 0.0])
        cvec = np.array([C * cb, C * c1, C * np.sqrt(1 - cb * cb - c1 * c1)])

        for vec in [avec, bvec, cvec]:
            for i in range(3):
                if np.isclose(vec[i], 0.0, atol=0.0001):
                    vec[i] = 0.0
        
        return avec, bvec, cvec

    def map_back(chain, model, tol=2.0):
        print("mapping back chain {}...".format(c+1))
        found = False

        options = []
        for uc_translation in uc_t:
            for i in range(per_unit_cell):
                options.append((uc_translation, i))

        UC_T, RMAT, TVEC = (None, None, None)
        for option in options:
            working_chain = chain.detached_copy()
            for atom in working_chain.atoms():
                atom.set_xyz(tuple(np.array(atom.xyz)+option[0]))
            xyz = working_chain.atoms().extract_xyz()
            coords_frac = xrs.crystal_symmetry().unit_cell().fractionalize(xyz)
            new_xyz = xrs.crystal_symmetry().unit_cell().orthogonalize(rmats[option[1]].elems * (coords_frac - tvecs[option[1]]))
            working_chain.atoms().set_xyz(new_xyz)
            avg_coord = np.mean(np.array(new_xyz), axis=0)
            if np.linalg.norm(avg_coord-avg_coord_xrs) < tol:
                UC_T = option[0]
                RMAT = rmats[option[1]]
                TVEC = tvecs[option[1]]
                break

        assert (UC_T is not None) and (RMAT is not None) and (TVEC is not None), "not able to find a mapping back on to the asymmetric unit. Try increasing the tolerace (tol) in the method map_back (default: tol=2.0)"
        
        # move unit cells back to original unit cell position
        # and reset i_seq
        for atom in chain.atoms():
            atom.set_xyz(tuple(np.array(atom.xyz) + UC_T))

        # then reverse the space group symmetry operations
        xyz = chain.atoms().extract_xyz()
        xyz = xrs.crystal_symmetry().unit_cell().fractionalize(xyz)
        new_xyz = xrs.crystal_symmetry().unit_cell().orthogonalize(RMAT.elems * (xyz - TVEC))
        chain.atoms().set_xyz(new_xyz)

        resid = 1
        for new_res in chain.residue_groups():
            new_res.resseq = resid
            resid += 1
    
    # load in the files for the supercell and crystal structure
    supercell_pdb = iotbx.pdb.input(file_name=supercell_pdb_filename)
    xrs_pdb       = iotbx.pdb.input(file_name=xrs_pdb_filename)
    SC  = supercell_pdb.construct_hierarchy(sort_atoms=False)
    xrs = xrs_pdb.xray_structure_simple()
    
    # unit cell vectors
    avec, bvec, cvec = _get_unit_cell_vectors(xrs)
    avec_SC, bvec_SC, cvec_SC = _get_unit_cell_vectors(supercell_pdb.xray_structure_simple())
    
    # number of unit cells along each axis and total number of unit cells in supercell
    nA = int(np.linalg.norm(avec_SC)/np.linalg.norm(avec))
    nB = int(np.linalg.norm(bvec_SC)/np.linalg.norm(bvec))
    nC = int(np.linalg.norm(cvec_SC)/np.linalg.norm(cvec))
    num_unit_cells = nA * nB * nC
    
    # space group and symmetry operations
    space_group = xrs.crystal_symmetry().space_group()
    space_group_ops = space_group.all_ops()
    
    # chains per unit cell
    per_unit_cell = len(space_group_ops)
    
    # residues and atoms per copy of the molecule
    residues_per_copy = int(SC.overall_counts().n_residues/(num_unit_cells * per_unit_cell))
    atoms_per_copy = int(SC.overall_counts().n_atoms/(num_unit_cells * per_unit_cell))
    
    # space group symmetry rotation matrices and translation vectors
    rmats = [scitbx.matrix.sqr(m.r().as_double()) for m in space_group_ops]
    tvecs = [m.t().as_double() for m in space_group_ops]

    # inverse unit translation vector for each cell
    # i.e. the vector that sends each cell back
    # to the asu-containing unit cell
    iut_cells = -1 * np.array([el for el in itertools.product(range(nA), range(nB), range(nC))])

    # unit cell translation vectors
    uc_t = []
    for indices in iut_cells:
        uc_t.append(avec * indices[0] + bvec * indices[1] + cvec * indices[2])

    # chain ids for the ensemble
    idl = [char for char in string.ascii_lowercase]
    idu = [char for char in string.ascii_uppercase]
    chain_ids = idl + idu
    
    # average coordinate of the x-ray structure
    avg_coord_xrs = np.mean(np.array([atom.xyz for atom in xrs_pdb.construct_hierarchy().atoms()]), axis=0)

    # empty ensemble objects
    ens = iotbx.pdb.hierarchy.root()
    
    # chain and atom counters
    c = 0
    atom_count = 0
    
    # first empty model and chain
    model = iotbx.pdb.hierarchy.model(id="{}".format(c+1))
    chain = iotbx.pdb.hierarchy.chain(id=chain_ids[c])
    
    # go through the supercell, filling the chains/models
    # until you reach the end of each protein
    for sc_chain in SC.chains():
        for res in sc_chain.residue_groups():
            res_atoms = [a for a in res.atoms()] 
            atom_count += len(res_atoms)
            # keep appending residues if we haven't reached the end of the chain
            if (atom_count-1) // atoms_per_copy == c:
                chain.append_residue_group(res.detached_copy())

            else:
                # find the right symmetry operations to map back onto ASU
                map_back(chain, model)
                    
                # add the model to the ensemble
                model.append_chain(chain)
                ens.append_model(model)
                
                # increment the chain counter
                c = (atom_count-1) // atoms_per_copy
                
                # create a new empty model and chain
                model = iotbx.pdb.hierarchy.model(id="{}".format(c+1))
                chain = iotbx.pdb.hierarchy.chain(id=chain_ids[c])
                
                chain.append_residue_group(res.detached_copy())

    # last chain
    map_back(chain, model)
    model.append_chain(chain)
    ens.append_model(model)
    
    # clean things up
    ens.atoms_reset_serial()
    
    return ens

# get the ensemble and write it out
ensemble = reverse_propagate_supercell(sc_pdb, xrs_pdb)
xtal_symmetry = iotbx.pdb.input(file_name=xrs_pdb).xray_structure_simple()
ensemble.write_pdb_file(outpdb, crystal_symmetry=xtal_symmetry)

