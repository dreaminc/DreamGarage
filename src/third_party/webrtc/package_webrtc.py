from __future__ import unicode_literals
from os.path import join, isfile
import shutil
import os, shutil, sys


# This will parse through a webrtc checkout directory and copy 
# over the include headers as well as all of the required libraries 

# usage example:
# 	package_webrtc.py <webrtc src directory> <destination directory>

def copyFilesInTree(cpy_src_dir, cpy_dest_dir, file_ext):
	cpy_src_dir = unicode(cpy_src_dir)
	cpy_dest_dir = unicode(cpy_dest_dir)
	
	print("Copying {0} files from {1} to {2}".format(file_ext, cpy_src_dir, cpy_dest_dir))
	
	ignore_func = lambda d, files: [f for f in files if isfile(join(d, f)) and not f.endswith(file_ext)]
	
	try:
	    shutil.copytree(cpy_src_dir, cpy_dest_dir, ignore=ignore_func)	
	except shutil.Error as err:
	    errors.extend(err.args[0])
	except OSError:
		pass

	

def main():
	# src_dir is the src folder of a webrtc checkout
	src_dir = sys.argv[1] 
	
	# dest_dir is the location to generate the package
	dest_dir = sys.argv[2]
	
	errors = []
	
	print("Packaging includes")
	includes_src_dir = src_dir
	includes_dest_dir = dest_dir + "/include/webrtc/"
	copyFilesInTree(includes_src_dir, includes_dest_dir, '.h')
	
	print("Packaging Debug")
	debug_src_dir = src_dir+ "/out/Debug_x64/"
	debug_dest_dir = dest_dir + "/lib/x64/Debug/"
	copyFilesInTree(debug_src_dir, debug_dest_dir, '.lib')
	
	print("Packaging Release")
	release_src_dir = src_dir + "/out/Release_x64/"
	release_dest_dir = dest_dir + "/lib/x64/Release/"
	copyFilesInTree(release_src_dir , release_dest_dir, '.lib')
	
	print("Packaging json includes")
	json_src_dir = src_dir + "/third_party/jsoncpp/source/include/"
	json_dest_dir = dest_dir + "/include/jsoncpp/"
	copyFilesInTree(json_src_dir, json_dest_dir, '.h')
	
	print("Packaging debug json obj")
	json_src_dir = src_dir + "/out/Debug_x64/obj/third_party/jsoncpp/jsoncpp"
	json_dest_dir = dest_dir + "/lib/x64/Debug/jsoncpp"
	copyFilesInTree(json_src_dir, json_dest_dir, '.obj')
	
	print("Packaging release json obj")
	json_src_dir = src_dir + "/out/Release_x64/obj/third_party/jsoncpp/jsoncpp"
	json_dest_dir = dest_dir + "/lib/x64/Release/jsoncpp"
	copyFilesInTree(json_src_dir, json_dest_dir, '.obj')
	
	# Print out errors
	if errors:
		raise shutil.Error(errors)

	
# execute only if run as a script
if __name__ == "__main__":
    
    main()