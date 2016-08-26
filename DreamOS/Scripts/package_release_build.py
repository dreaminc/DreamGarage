# packaging production binaries from Release build

from os.path import join, isfile
from shutil import copytree, copyfile
import os, shutil, sys

# src_dir is the root for Dream
src_dir = sys.argv[1] 

# dest_dir is the target location of Release build
dest_dir = sys.argv[2]

# copy only necessary files from Dream root necessary to run

copyfile(src_dir + "/dreampaths.txt", dest_dir + "/dreampaths.txt")

print("Copying HAL")
try:
    ignore_func = lambda d, files: [f for f in files if isfile(join(d, f)) and (f[-5:] != '.frag' and f[-5:] != '.vert')]
    copytree(src_dir + "/HAL/", dest_dir + "/HAL/", ignore=ignore_func)
except FileExistsError:
    print("Already Copied")


print("Copying Models")
try:
    copytree(src_dir + "/Models/", dest_dir + "/Models/")
except FileExistsError:
    print("Already Copied")


print("Copying Resources")
try:
    copytree(src_dir + "/Resources/", dest_dir + "/Resources/")
except FileExistsError:
    print("Already Copied")

print("Copying Textures")
try:
    copytree(src_dir + "/Texture/", dest_dir + "/Texture/")
except FileExistsError:
    print("Already Copied")
