import libtbx.load_env
import os
op = os.path
Import("env_base", "env_etc")

env_etc.lunus_dist = libtbx.env.dist_path("lunus")
env_etc.lunus_include = os.path.dirname(env_etc.lunus_dist)
env_etc.lunus_common_includes = [
  env_etc.libtbx_include,
  env_etc.scitbx_include,
  env_etc.lunus_include,
  env_etc.boost_adaptbx_include,
  env_etc.boost_include,
  op.dirname(libtbx.env.find_in_repositories(
    relative_path="tbxx", optional=False))
]

if (not env_etc.no_boost_python):
  Import("env_no_includes_boost_python_ext")
  env_lunus = env_no_includes_boost_python_ext.Clone()
  
  env_etc.include_registry.append(
    env=env_lunus,
    paths=env_etc.lunus_common_includes + [env_etc.python_include])
  env_lunus.Append(LIBS=env_etc.libm+["scitbx_boost_python","boost_python","cctbx"])

  env_etc.enable_more_warnings(env=env_lunus)
  env_lunus.SConscript("lunus/SConscript",exports={ 'env' : env_lunus })
