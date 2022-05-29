# lunusmd Prototype MD code
# Author: Michael E. Wall, Los Alamos National Laboratory
import io
import pandas as pd

def getsection(lines,label,skip=0):
    insection = False
    inheader = False
    headct = 0
    for line in lines:
        if (line.find(label) >= 0):
            inheader = True
        if inheader:
            if (headct == 1 + skip):
                inheader = False
                insection = True
            else:
                headct += 1
        if insection:
            if line.isspace():
                break
            else:
                yield line

def get_atom_table(lines):
    with io.StringIO() as f:
        for l in getsection(lines,"Atom Definition Parameters",3):
            f.write(l)
        f.seek(0)
        colspecs = [(0,6),(11,14),(14,21),(21,28),(28,34),(34,44),(44,49),(54,78)]
        table = pd.read_fwf(f,colspecs=colspecs,header=None,names=['Index','Symbol','Type','Class','Atomic','Mass','Valence','Description'],index_col=0)
        return table

def get_bond_table(lines):
    with io.StringIO() as f:
        for l in getsection(lines,"Bond Stretching Parameters",3):
            f.write(l)
        f.seek(0)
        colspecs = [(0,6),(9,15),(15,21),(40,50),(50,60)]
        table = pd.read_fwf(f,colspecs=colspecs,header=None,names=['Index','Atom1','Atom2','KS','Bond'],index_col=0)
        return table

def get_angle_table(lines):
    with io.StringIO() as f:
        for l in getsection(lines,"Angle Bending Parameters",3):
            f.write(l)
        f.seek(0)
        colspecs = [(0,6),(9,15),(15,21),(21,27),(40,50),(50,60)]
        table = pd.read_fwf(f,colspecs=colspecs,header=None,names=['Index','Atom1','Atom2','Atom3','KB','Angle'],index_col=0)
        return table

def get_stretch_bend_table(lines):
    with io.StringIO() as f:
        for l in getsection(lines,"Stretch-Bend Parameters",3):
            f.write(l)
        f.seek(0)
        colspecs = [(0,6),(9,15),(15,21),(21,27),(28,38),(38,48),(50,59),(59,68),(68,77)]
        table = pd.read_fwf(f,colspecs=colspecs,header=None,names=['Index','Atom1','Atom2','Atom3','KSB1','KSB2','Angle','Bond1','Bond2'],index_col=0)
        return table

def get_urey_bradley_table(lines):
    with io.StringIO() as f:
        for l in getsection(lines,"Urey-Bradley Parameters",3):
            f.write(l)
        f.seek(0)
        colspecs = [(0,6),(9,15),(15,21),(21,27),(40,50),(50,60)]
        table = pd.read_fwf(f,colspecs=colspecs,header=None,names=['Index','Atom1','Atom2','Atom3','KUB','Distance'],index_col=0)
        return table

def get_vdw_table(lines):
    with io.StringIO() as f:
        for l in getsection(lines,"Van der Waals Parameters",3):
            f.write(l)
        f.seek(0)
        colspecs = [(0,6),(9,15),(22,32),(32,42),(43,53),(53,63),(64,74)]
        table = pd.read_fwf(f,colspecs=colspecs,header=None,names=['Index','AtomNumber','Size','Epsilon','Size14','Epsilon14','Reduction'],index_col=0)
        return table

def get_multipole_table(lines):
    with io.StringIO() as f:
        linect = 0
        lconcat = ""
        for l in getsection(lines,"Atomic Multipole Parameters",3):
            lconcat += l.rstrip('\n')
            linect += 1
            if (linect == 5):
                f.write(lconcat+'\n')
                linect = 0
                lconcat = ""
        f.seek(0)
        colspecs = [(0,6),(9,15),(16,23),(23,30),(30,37),(40,48),(50,59),(109,118),(118,127),(127,136),(186,195),(245,254),(254,263),(313,322),(322,331),(331,340)]
        table = pd.read_fwf(f,colspecs=colspecs,header=None,names=['Index','Atom','ZAxis','XAxis','YAxis','Frame','Charge','D1','D2','D3','Q1','Q2','Q3','Q4','Q5','Q6'],index_col=0)
        return table

