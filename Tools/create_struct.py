import re
import sys
import os
import datetime

if len(sys.argv) > 2:
    # The first parameter passed in is the structure name.
    project_path = sys.argv[1]
    structure_name = sys.argv[2]
    
    # Only allow lowercase characters and the underscore in structure names.
    structure_name = re.sub("[^a-z_]", "", structure_name)
    upper_structure_name = str.upper(structure_name)
    
    # Check to make sure that the string is now longer than 1 character.
    if len(structure_name) <= 1:
        print("Too few characters or bad structure name.")
        sys.exit()
        
    # Create a new header file with the name based on the structure name
    header_filename = project_path + "\\" + structure_name + ".h"
    code_filename = project_path + "\\" + structure_name + ".c"
    
    # If the file exists then exit.
    if os.path.isfile(header_filename) or os.path.isfile(code_filename):
        print("File already exists " + header_filename + " " + code_filename)
        sys.exit()
        
    # Open the files.
    header_file = open(header_filename, 'w')
    code_file = open(code_filename, 'w')
    
    # Current date.
    now = datetime.datetime.now()
    
    # Write the comment headers for the files.
    header_file.write("/*\n")
    header_file.write(" * " + structure_name + ".h\n")
    header_file.write(" *\n")
    header_file.write(" *  Created on: " + now.strftime("%d %b %Y") + "\n")
    header_file.write(" *      Author: David Tyler\n")
    header_file.write(" */\n\n")
    
    code_file.write("/*\n")
    code_file.write(" * " + structure_name + ".c\n")
    code_file.write(" *\n")
    code_file.write(" *  Created on: " + now.strftime("%d %b %Y") + "\n")
    code_file.write(" *      Author: David Tyler\n")
    code_file.write(" */\n\n")
    code_file.write("#include \"includes.h\"\n\n")
    
    # Write the inclusion guard at the top of the header file.
    header_file.write("#ifndef " + upper_structure_name + "_H_\n")
    header_file.write("#define " + upper_structure_name + "_H_\n")
    header_file.write("\n")
    header_file.write("typedef struct " + structure_name + "\n")
    header_file.write("{\n")
    header_file.write("\n")
    header_file.write("} " + upper_structure_name + ";\n")
    header_file.write("\n")
    header_file.write("#endif /* " + upper_structure_name + "_H_ */")
    
    code_file.write("/*\n")
    code_file.write(" * create_" + structure_name + "\n")
    code_file.write(" *\n")
    code_file.write(" * Allocates the memory required for a " + structure_name + "\n")
    code_file.write(" *\n")
    code_file.write(" * Returns: A pointer to the newly created memory\n")
    code_file.write(" */\n\n")
    code_file.write(upper_structure_name + " *create_" + structure_name + "()\n")
    code_file.write("{\n")
    code_file.write("  /*\n")
    code_file.write("   * Local Variables\n")
    code_file.write("   */\n")
    code_file.write("  " + upper_structure_name + " *" + structure_name + ";\n")
    code_file.write("\n")
    code_file.write("  /*\n")
    code_file.write("   * Allocate the required memory\n")
    code_file.write("   */\n")
    code_file.write("  " + structure_name + " = (" + upper_structure_name + " *) DT_MALLOC(sizeof(" + upper_structure_name + "));\n")
    code_file.write("\n")
    code_file.write("  return(" + structure_name + ");\n")
    code_file.write("}\n")
    code_file.write("\n")
    
    code_file.write("/*\n")
    code_file.write(" * destroy_" + structure_name + "\n")
    code_file.write(" *\n")
    code_file.write(" * Frees the memory used by the passed in object.\n")
    code_file.write(" *\n")
    code_file.write(" * Parameters: " + structure_name + " - The object to be freed.\n")
    code_file.write(" */\n")
    code_file.write("void destroy_" + structure_name + "(" + upper_structure_name + " *" + structure_name + ")\n")
    code_file.write("{\n")
    code_file.write("  /*\n")
    code_file.write("   * Free the object.\n")
    code_file.write("   */\n")
    code_file.write("  DT_FREE(" + structure_name + ");\n")
    code_file.write("}\n")
    
    
    header_file.close()
    code_file.close()