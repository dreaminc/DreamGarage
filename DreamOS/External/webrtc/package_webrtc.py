from os.path import join, isfile
from shutil import copytree
import os, shutil, sys

# packaging webrtc for Dream
# usage example:
# 	package_webrtc.py c:\work\webrtc-checkout\src c:\work\webrtc_package

# src_dir is the src folder of a webrtc checkout
src_dir = sys.argv[1] 

# dest_dir is the location to generate the package
dest_dir = sys.argv[2]

print "Packaging includes"
ignore_func = lambda d, files: [f for f in files if isfile(join(d, f)) and f[-2:] != '.h']
copytree(src_dir + "/webrtc/", dest_dir + "/include/webrtc/", ignore=ignore_func)

print "Packaging Debug x64"
ignore_func = lambda d, files: [f for f in files if isfile(join(d, f)) and f[-4:] != '.lib']
copytree(src_dir + "/out/Debug_x64/", dest_dir + "/lib/x64/Debug/", ignore=ignore_func)

print "Packaging Release x64"
ignore_func = lambda d, files: [f for f in files if isfile(join(d, f)) and f[-4:] != '.lib']
copytree(src_dir + "/out/Release_x64/", dest_dir + "/lib/x64/Release/", ignore=ignore_func)

print "Packaging json"
#ignore_func = lambda d, files: [f for f in files if isfile(join(d, f)) and f[-2:] != '.h']
#copytree(src_dir + "/third_party/jsoncpp/overrides/include/", dest_dir + "/include/jsoncpp/", ignore=ignore_func)
ignore_func = lambda d, files: [f for f in files if isfile(join(d, f)) and f[-2:] != '.h']
copytree(src_dir + "/third_party/jsoncpp/source/include/", dest_dir + "/include/jsoncpp/", ignore=ignore_func)
