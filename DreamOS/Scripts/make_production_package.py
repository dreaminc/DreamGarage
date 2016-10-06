# This will create a production package derived from the Release build by removing the unnecessary files needed
# for Dream to run.

from os.path import join, isfile
from shutil import copytree, copyfile, rmtree
import os, shutil, sys

dir = os.path.dirname(__file__)

# src_dir is the root for Dream
src_dir = "../Project/Windows/DreamOS/x64/Production"
#src_dir = os.path.join(dir, src_dir)

# dest_dir is the target location of Production build
dest_dir = "../Project/Windows/Production"
#dest_dir = os.path.join(dir, dest_dir)

#if os.path.isdir(dest_dir):
#   print(dest_dir + ' exists.')
#else:
#   os.mkdir(dest_dir)
#   print(dest_dir + ' created.')

# copy only necessary files from Dream root necessary to run

try:
    rmtree(dest_dir)
except FileNotFoundError:
    print("Already empty")

print("Copying binaries")
try:
    ignore_func = lambda d, files: [f for f in files if isfile(join(d, f)) and (f[-4:] != '.exe' and f[-4:] != '.dll')]
    copytree(src_dir + "/", dest_dir + "/", ignore=ignore_func)
except FileExistsError:
    print("Already Copied")

rmtree(dest_dir + "/DreamOS.tlog")

copyfile(src_dir + "/dreampaths.txt", dest_dir + "/dreampaths.txt")

rmtree(dest_dir + "/HAL")
print("Copying HAL")
try:
    ignore_func = lambda d, files: [f for f in files if isfile(join(d, f)) and (f[-5:] != '.frag' and f[-5:] != '.vert')]
    copytree(src_dir + "/HAL/", dest_dir + "/HAL/", ignore=ignore_func)
except FileExistsError:
    print("Already Copied")

rmtree(dest_dir + "/Models")
print("Copying Models")
try:
    copytree(src_dir + "/Models/", dest_dir + "/Models/")
except FileExistsError:
    print("Already Copied")

rmtree(dest_dir + "/Resources")
print("Copying Resources")
try:
    copytree(src_dir + "/Resources/", dest_dir + "/Resources/")
except FileExistsError:
    print("Already Copied")

rmtree(dest_dir + "/Texture")
print("Copying Textures")
try:
    copytree(src_dir + "/Texture/", dest_dir + "/Texture/")
except FileExistsError:
    print("Already Copied")
