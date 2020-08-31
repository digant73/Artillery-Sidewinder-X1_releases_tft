Import("env")
import os
from distutils.dir_util import copy_tree, remove_tree, mkpath
import shutil 

def after_build(source, target, env):
    dst_dir = env['PROJECT_DIR'] + "/Copy to SD Card - MKS"
    # Remove any existing Folder
#    if os.path.exists(dst_dir):
#        remove_tree(dst_dir)
#        mkpath(dst_dir)
    # Copy BMP folder to SD Card Folder
#    src_dir = env['PROJECT_DIR'] + "/Copy to SD Card root directory to update - Unified Menu Material theme/MKS"
#    copy_tree(src_dir, dst_dir + "/MKS")
    # Copy Bin File to SD Card Folder
    src_filename = env['PROJECT_BUILD_DIR'] + "/" + env['PIOENV'] + "/" + env['PROGNAME'] + ".bin"
    dst_filename = dst_dir + "/" + env['PROGNAME'] + ".bin"
    shutil.copyfile(src_filename, dst_filename)
env.AddPostAction("buildprog", after_build)
