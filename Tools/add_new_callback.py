import os
import sys
import re

################################################################################
# add_new_lua_callback                                                         #
#                                                                              #
# Adds a new callback from lua to the game.                                    #
#                                                                              #
# Parameters: callback - The name of the callback function.                    #
################################################################################
def add_new_lua_callback(callback):
    callback_name_file_in = open("..\\resources\\automaton\\lua_callbacks.txt", "r")
    callback_code_file_in = open("..\\src\\impl_automatons\\lua_callbacks\\lua_call_back_functions.c","r")
    callback_code_file_out = open("..\\src\\impl_automatons\\lua_callbacks\\lua_call_back_functions.c.out","w")
    
    found_callback_line = False
    for line in callback_name_file_in:
        if line == callback + "\n":
            found_callback_line = True
            break
    callback_name_file_in.close()
    
    if not found_callback_line:
        callback_name_file_out = open("..\\resources\\automaton\\lua_callbacks.txt", "a")
        callback_name_file_out.write(callback + "\n")
        callback_name_file_out.close()
    
    # Read all the lines into a list so that we can edit them in place.
    callback_code_lines = callback_code_file_in.readlines()
    callback_code_file_in.close()
    
    # Add an entry into the register callbacks function.
    first_callback = callback_code_lines.index("  lua_register(lua_state,\n")
    temp_lines = callback_code_lines[:first_callback]
    temp_lines.append('  lua_register(lua_state,\n')
    temp_lines.append('               "callback_' + callback + '",\n')
    temp_lines.append('               lua_callback_' + callback + ');\n')
    temp_lines.extend(callback_code_lines[first_callback:])
              
    # Add the callback function stub to be filled in later. This goes directly
    # above the register callbacks function.
    register_callback_index = temp_lines.index(" * register_lua_callback_functions\n")
    output_lines = temp_lines[:register_callback_index - 1]
    output_lines.append('/*\n')
    output_lines.append(' * lua_callback_' + callback + '\n')
    output_lines.append(' *\n')
    output_lines.append(' * TODO: Fill in function definition for callback')
    output_lines.append(' */\n')
    output_lines.append('int lua_callback_' + callback + '(lua_State *lua_state)\n')
    output_lines.append('{\n')
    output_lines.append('  // TODO: Fill in callback function ' + callback + '\n')
    output_lines.append('}\n\n')
    output_lines.extend(temp_lines[register_callback_index - 1:])
    
    for line in output_lines:
        callback_code_file_out.write(line)
    callback_code_file_out.close()
    os.remove("..\\src\\impl_automatons\\lua_callbacks\\lua_call_back_functions.c")
    os.rename("..\\src\\impl_automatons\\lua_callbacks\\lua_call_back_functions.c.out",
              "..\\src\\impl_automatons\\lua_callbacks\\lua_call_back_functions.c")
    
if len(sys.argv) == 2:
    add_new_lua_callback(sys.argv[1])
else:
    print "Wrong number of parameters"