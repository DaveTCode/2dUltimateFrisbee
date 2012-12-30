################################################################################
# create_make_file                                                             #
#                                                                              #
# Creates make files for use with gcc and this project. Done to avoid having   #
# to ever have to update the make file. The only thing that will require       #
# updating here is the list of excluded directories/files.                     #
#                                                                              #
# It is assumed that this file is run from the Tools directory in the project. #
################################################################################
import os

EXCLUDED_DIRS = ['.settings', '.svn', 'Debug', 'Release', 'resources', 'Tools', 'ezxml']
EXCLUDED_FILES = []

h_files = []
c_files = []

for root, dirs, files in os.walk(".."):
    for file in files:
        if file.endswith(".c"):
            c_files.append(os.path.join(root,file))
        elif file.endswith(".h"):
            h_files.append(os.path.join(root,file))

# Create the make file, overwriting if it already exists.
make_file = open(os.path.join("..", "makefile"), "w")

make_file.write(