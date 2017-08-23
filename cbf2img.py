from dxtbx.format.Registry import Registry
import sys, os

f = None
for arg in sys.argv:
  if arg.split(".")[-1].lower() == "cbf" and os.path.exists(arg):
    if f is None:
      f = Registry.find(arg)
    img = f(arg)
    db = img.get_detectorbase()
    db.readHeader()
    db.read()
    db.show_header()
    destpath = arg.rstrip(".cbf") + ".img"
    print "Writing %s as %s"%(arg,destpath)

    db.debug_write(destpath)
