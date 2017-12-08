from os.path import join, isfile
import shutil
import os, shutil, sys

# This will parse through a webrtc checkout directory and copy 
# over the include headers as well as all of the required libraries 

# usage example:
# 	package_webrtc.py <webrtc src directory> <destination directory>

# src_dir is the src folder of a webrtc checkout
src_dir = sys.argv[1] 

# dest_dir is the location to generate the package
dest_dir = sys.argv[2]

errors = []

print "Packaging includes"
ignore_func = lambda d, files: [f for f in files if isfile(join(d, f)) and f[-2:] != '.h']

# TODO: Move into function
try:
	shutil.copytree(src_dir, dest_dir + "/include/webrtc/", ignore=ignore_func)	
except shutil.Error as err:
    errors.extend(err.args[0])
except OSError as err:
    errors.extend(err.args[0])

print "Packaging Debug x64"
ignore_func = lambda d, files: [f for f in files if isfile(join(d, f)) and f[-4:] != '.lib']
shutil.copytree(src_dir + "/out/Debug_x64/", dest_dir + "/lib/x64/Debug/", ignore=ignore_func)

print "Packaging Release x64"
ignore_func = lambda d, files: [f for f in files if isfile(join(d, f)) and f[-4:] != '.lib']
shutil.copytree(src_dir + "/out/Release_x64/", dest_dir + "/lib/x64/Release/", ignore=ignore_func)

print "Packaging json"
#ignore_func = lambda d, files: [f for f in files if isfile(join(d, f)) and f[-2:] != '.h']
#copytree(src_dir + "/third_party/jsoncpp/overrides/include/", dest_dir + "/include/jsoncpp/", ignore=ignore_func)
ignore_func = lambda d, files: [f for f in files if isfile(join(d, f)) and f[-2:] != '.h']
shutil.copytree(src_dir + "/third_party/jsoncpp/source/include/", dest_dir + "/include/jsoncpp/", ignore=ignore_func)

# Print out errors
if errors:
	raise shutil.Error(errors)
