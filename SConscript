   # Authors: Mike Wall & Nick Sauter
   # Date: 6/15/2017
   # Lattice methods wrapped by Alex Wolff (9/22/2017).





import libtbx.load_env
import os, sys
from libtbx.env_config import get_boost_library_with_python_version
op = os.path
Import("env_base", "env_etc")

env_etc.lunus_dist = libtbx.env.dist_path("lunus")

env_etc.lunus_include = env_etc.lunus_dist
env_etc.lunus_common_includes = [
  env_etc.libtbx_include,
  env_etc.scitbx_include,
  env_etc.lunus_include,
  env_etc.boost_adaptbx_include,
  env_etc.boost_include,
  op.dirname(libtbx.env.find_in_repositories(
    relative_path="tbxx", optional=False))
]

CPPP = os.path.join(env_etc.lunus_include,"c","include")

env_lunus = env_base.Clone(SHLINKFLAGS=env_etc.shlinkflags)

if "-ffast-math" in env_lunus["CCFLAGS"]:
  replacement_ccflags = []
  for f in env_lunus["CCFLAGS"]:
    if (f not in ["-ffast-math"]):
      replacement_ccflags.append(f)
  env_lunus.Replace(CCFLAGS = replacement_ccflags)

if "-ffast-math" in env_lunus["CCFLAGS"]:
  replacement_shcxxflags = []
  for f in env_lunus["SHCXXFLAGS"]:
    if (f not in ["-ffast-math"]):
      replacement_shcxxflags.append(f)
  env_lunus.Replace(SHCXXFLAGS = replacement_shcxxflags)

if (env_etc.have_openmp):
  env_lunus.Prepend(CCFLAGS=["-DUSE_OPENMP"])
  env_lunus.Prepend(SHCXXFLAGS=["-DUSE_OPENMP"])
if sys.platform.startswith('linux') and env_etc.enable_kokkos:
  kokkos_flags = ["-DUSE_KOKKOS","-DLUNUS_NUM_JBLOCKS=16","-DLUNUS_NUM_IBLOCKS=8"]
  env_lunus.Prepend(CCFLAGS=kokkos_flags)
  env_lunus.Prepend(SHCXXFLAGS=kokkos_flags)
else:
  if (env_etc.enable_cuda):
    env_lunus.Prepend(CCFLAGS=["-DUSE_CUDA","-DLUNUS_NUM_JBLOCKS=16","-DLUNUS_NUM_IBLOCKS=8"])

correct_prefix = "#"+os.path.basename(env_etc.lunus_dist)

env_lunus.StaticLibrary(target='#lib/lunus',
  source = [os.path.join(correct_prefix,"c","lib",srcfile) for srcfile in [
  "linitim.c",
  "lcalcsim.c",
  "lfreeim.c",
  "lmodeim.c",
  "lofstim.c",
  "lpunchim.c",
  "lnormim.c",
  "lpolarim.c",
  "lscaleim.c",
  "lavgrcf.c",
  "lavgrim.c",
  "lwindim.c",
  "lthrshim.c",
  "linitlt.c",
  "lfreelt.c",
  "lreadlt.c",
  "lreadvtk.c",
  "lgettag.c",
  "lparsecelllt.c",
  "lmattools.c",
  "lwritevtk.c",
  "lwritehkl.c",
  "lwritelt.c",
  "lsetmetim.c",
  "lsymlt.c",
  "lsymlib.c",
  "lanisolt.c",
  "lsvtools.c",
  "lspline.c",
  "lsetparamsim.c",
  "lsetparamslt.c",
  "lsubrfim.c",
  "ltime.c",
  "lprocimlt.c",
  "lcloneim.c",
  "lcfim.c",
  "lwritecube.c",
  "lbkgsubim.c",
  "lslistim.c",
  "lccrlt.c",
  "lcorrlt.c",
  "linitmap.c",
  "lwritemap.c",
  "lreadcube.c",
  "lreadhkl.c",
  "lscalelt.c",
  "lmulsclt.c",
  "lmulcfim.c"]],
  CPPPATH=[CPPP] )

for f in [os.path.join(correct_prefix,"c","src",srcfile) for srcfile in [
    "anisolt.c",
    "corrlt.c",
    "ccrlt.c",
    "symlt.c",
    "hkl2lat.c",
    "makelt.c",
    "scalelt.c",
    "mulsclt.c",
    "cube2map.c",
    "vtk2cube.c",
    "lat2vtk.c"]]:
    env_lunus.Program('#bin/{}'.format(os.path.basename(f).split('.')[0]),f,LIBS=['lunus','m'],CPPPATH=[CPPP])

if (not env_etc.no_boost_python):

  Import("env_no_includes_boost_python_ext")
  env_lunus = env_no_includes_boost_python_ext.Clone()

  env_etc.include_registry.append(
    env=env_lunus,
    paths=env_etc.lunus_common_includes + [env_etc.python_include])

  if libtbx.env.build_options.use_conda:
    boost_python = get_boost_library_with_python_version(
        "boost_python", env_etc.conda_libpath
    )
  else:
    boost_python = "boost_python"

  env_lunus.Append(LIBS=env_etc.libm+["scitbx_boost_python",boost_python,"cctbx"])

  env_etc.enable_more_warnings(env=env_lunus)
  env_lunus.SConscript("lunus/SConscript",exports={ 'env' : env_lunus })
  if sys.platform.startswith('linux') and env_etc.enable_kokkos:
    env_lunus.SConscript("lunus/kokkos/SConscript",exports={ 'env' : env_lunus })
  else:
    if sys.platform.startswith('linux') and env_etc.enable_cuda:
      env_lunus.SConscript("lunus/cuda/SConscript",exports={ 'env' : env_lunus })

