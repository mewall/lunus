import sys

with open("./data/full_sys_start.pdb", 'r') as f:
    with open("full_sys_start_calphas.pdb", 'w') as g:
        for line in f.readlines():
            if line[:6] == "CRYST1":
                g.write(line)
            if line[:4] == "ATOM":
                if line[12:16].strip() == "CA":
                    g.write(line)
            else:
                pass
