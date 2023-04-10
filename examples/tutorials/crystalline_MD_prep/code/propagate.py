import iotbx.pdb
import cctbx
import numpy as np
import argparse
import itertools

def construct_supercell(UC, xrs, n_cells):
    '''Constructs a supercell from a unit cell given a number of cells in each dimension
    
    Parameters
    ----------
    UC : iotbx_pdb_hierarchy_ext.root)
        cctbx hierarchy object for the unit cell
    
    xrs : cctbx.xray.structure.structure
        cctbx xray structure object for the unit cell
    
    n_cells : int or tuple
        If given an int, N -> NxNxN supercell
        If given a tuple, (X,Y,Z) -> XxYxZ supercell
    
    Returns
    -------
    supercell : iotbx_pdb_hierarchy_ext.root
        cctbx hierarchy object for the supercell
    '''
    
    #calculate the unit cell vectors from the xray_structure
    A, B, C, alpha, beta, gamma = xrs.crystal_symmetry().unit_cell().parameters()
    cg = np.cos(np.radians(gamma))
    sg = np.sin(np.radians(gamma))
    cb = np.cos(np.radians(beta))
    ca = np.cos(np.radians(alpha))
    c1 = (ca - cb * cg) / sg
    avec = [A, 0.0, 0.0]
    bvec = [B*cg, B*sg, 0.0]
    cvec = [C*cb, C*c1, C*np.sqrt(1 - cb*cb - c1*c1)]
    for vec in [avec, bvec, cvec]:
        for i in range(3):
            if np.isclose(vec[i], 0.0, atol=0.0001):
                vec[i] = 0.0
    avec = np.array(avec)
    bvec = np.array(bvec)
    cvec = np.array(cvec)
    
    # start with a copy of the unitcell
    supercell = UC.deep_copy()
    
    # create an index tuple for each unit cell
    # e.g. (1,1,0) for the unitcell tiled forward
    #    in the x- and y-directions but not in the
    #    z-direction
    if not isinstance(n_cells, tuple):
        cells = [el for el in itertools.product(range(n_cells),repeat=3)]
    else:
        cells = [el for el in itertools.product(range(n_cells[0]), range(n_cells[1]), range(n_cells[2]))]
    
    for uc_idx in cells:
        
        # leave the original unit cell alone
        if uc_idx != (0,0,0):
            
            # create a unit cell copy
            new_UC = UC.deep_copy()
            
            # add a unit cell length along the appropriate axes to each atom
            for atom in new_UC.atoms():
                disp = uc_idx[0]*avec + uc_idx[1]*bvec + uc_idx[2]*cvec
                atom.set_xyz(tuple(np.array(atom.xyz)+disp))
            
            # add the chains from the shifted unit cell to the supercell
            for model in new_UC.models():
                supercell.models()[0].transfer_chains_from_other(model)
                    
    return supercell


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-au",  "--asymmetric_unit_pdb", help="name of asymmetric unit pdb file")
    parser.add_argument("-o",   "--output_pdb",          help="name of output supercell pdb file")
    parser.add_argument("-x", type=int, help="number of unit cells along the x-axis")
    parser.add_argument("-y", type=int, help="number of unit cells along the y-axis")
    parser.add_argument("-z", type=int, help="number of unit cells along the z-axis")
    args = parser.parse_args()
    
    au_pdb = args.asymmetric_unit_pdb
    out_pdb = args.output_pdb
    Nx = args.x
    Ny = args.y
    Nz = args.z
    
    input_pdb = iotbx.pdb.input(file_name=au_pdb)
    input_hierarchy = input_pdb.construct_hierarchy(sort_atoms=False)
    xray_structure = iotbx.pdb.input(file_name=au_pdb).xray_structure_simple()
    unit_cell = input_hierarchy.expand_to_p1(xray_structure.crystal_symmetry())
    
    A, B, C, alpha, gamma, beta = xray_structure.crystal_symmetry().unit_cell().parameters()
    space_group = xray_structure.crystal_symmetry().space_group_info().symbol_and_number()
    
    supercell_uc = cctbx.uctbx.unit_cell(parameters=(2*A,2*B,2*C,alpha,beta,gamma))
    supercell_symmetry = cctbx.crystal.symmetry(unit_cell=supercell_uc, space_group_symbol="P1") # <- may need to change this for your system

    supercell = construct_supercell(unit_cell, xray_structure, (Nx, Ny, Nz))
    supercell.write_pdb_file(out_pdb, crystal_symmetry=supercell_symmetry, anisou=False)
