import re
import os
import sys

################################################################################
# add_new_offensive_state                                                      #
#                                                                              #
# Adds a new state to the offensive automaton set.                             #
#                                                                              #
# Parameters: state - String name of the state.                                #
################################################################################
def add_new_offensive_state(state):    
    # Write the name of the state in a new line at the bottom of the state file.
    state_list_file = open("..\\resources\\automaton\\o_state_list.txt", "r")
    found_state_name = False
    for line in state_list_file:
        if line == state + "\n":
            found_state_name = True
            break
    state_list_file.close()
    if not found_state_name:
        state_list_file = open("..\\resources\\automaton\\o_state_list.txt", "a")
        state_list_file.write(state + "\n")
        state_list_file.close()
    
    # Write a function template for the new state in the bottom of the state
    # function file.
    state_func_file = open("..\\src\\impl_automatons\\o_automaton\\o_automaton_state_funcs.c", "r")
    found_state_function = False
    for line in state_func_file:
        if line.find("int state_" + state + "_function") != -1:
            found_state_function = True
            break
    state_func_file.close()
    if not found_state_function:
        state_func_file = open("..\\src\\impl_automatons\\o_automaton\\o_automaton_state_funcs.c", "a")
        state_func_file.write("\n/*\n")
        state_func_file.write(" * state_" + state + "_function\n")
        state_func_file.write(" *\n")
        state_func_file.write(" * TODO: Fill in function definition\n")
        state_func_file.write(" */\n")
        state_func_file.write("int state_" + state + "_function(PLAYER *player, Uint32 dt)\n")
        state_func_file.write("{\n")
        state_func_file.write("  // TODO: Fill in function for state " + state + "\n")
        state_func_file.write("}\n")
        state_func_file.write("\n")
        
        state_func_file.write("\n/*\n")
        state_func_file.write(" * state_" + state + "_start_function\n")
        state_func_file.write(" *\n")
        state_func_file.write(" * TODO: Fill in function definition\n")
        state_func_file.write(" */\n")
        state_func_file.write("int state_" + state + "_start_function(PLAYER *player)\n")
        state_func_file.write("{\n")
        state_func_file.write("  // TODO: Fill in start function for state " + state + "\n")
        state_func_file.write("}\n")
        state_func_file.write("\n")
        
        state_func_file.write("\n/*\n")
        state_func_file.write(" * state_" + state + "_exit_function\n")
        state_func_file.write(" *\n")
        state_func_file.write(" * TODO: Fill in function definition\n")
        state_func_file.write(" */\n")
        state_func_file.write("int state_" + state + "_exit_function(PLAYER *player)\n")
        state_func_file.write("{\n")
        state_func_file.write("  // TODO: Fill in end function for state " + state + "\n")
        state_func_file.write("}\n")
        state_func_file.write("\n")
        state_func_file.close()
    
    # Read the header file in and insert the definition of this state at the
    # correct point.
    state_header_file_in = open("..\\src\\impl_automatons\\o_automaton\\o_automaton_states.h", "r")
    state_header_file_out = open("..\\src\\impl_automatons\\o_automaton\\o_automaton_states.h.out", "w")
    written_header_line = False
    for line in state_header_file_in:        
        # Increment the total number of automaton states.
        if line.find("NUM_O_AUTOMATON_STATES") != -1:
            num_automatons_match = re.search("[0-9]+", line)
            num_automatons = int(num_automatons_match.group(0)) + 1
            output_line = "#define NUM_O_AUTOMATON_STATES " + str(num_automatons) + "\n"
        elif (line.find("O_AUTOMATON_STATE_") != -1) and (written_header_line == False):
            output_line = "#define O_AUTOMATON_STATE_" + state.upper() + ' "state_' + state + '"\n'
            output_line = output_line + line
            written_header_line = True
        else:
            output_line = line
            
        state_header_file_out.write(output_line)
        
    # Replace the old headers file with the new one.
    state_header_file_in.close()
    state_header_file_out.close()
    os.remove("..\\src\\impl_automatons\\o_automaton\\o_automaton_states.h")
    os.rename("..\\src\\impl_automatons\\o_automaton\\o_automaton_states.h.out", 
              "..\\src\\impl_automatons\\o_automaton\\o_automaton_states.h")
              
    # Read in the code file and add an entry in to the case statement to add 
    # this state.
    state_code_file_in = open("..\\src\\impl_automatons\\o_automaton\\o_automaton_states.c", "r")
    state_code_file_out = open("..\\src\\impl_automatons\\o_automaton\\o_automaton_states.c.out", "w")
    for line in state_code_file_in:
        if line.find("default:") != -1:
            output_line = "      case " + str(num_automatons - 1) + ":\n"
            output_line = output_line + "        strncpy((*state_array)[ii]->name,\n"
            output_line = output_line + "                O_AUTOMATON_STATE_" + state.upper() + ",\n"
            output_line = output_line + "                MAX_EVENT_NAME_LEN);\n"
            output_line = output_line + "        (*state_array)[ii]->state_function = state_" + state + "_function;\n"
            output_line = output_line + "        (*state_array)[ii]->entrance_function = state_" + state + "_start_function;\n"
            output_line = output_line + "        (*state_array)[ii]->exit_function = state_" + state + "_exit_function;\n"
            output_line = output_line + "        break;\n"
            output_line = output_line + line
        else:
            output_line = line
            
        state_code_file_out.write(output_line)
            
    # Replace the old code file with the new one.
    state_code_file_in.close()
    state_code_file_out.close()
    os.remove("..\\src\\impl_automatons\\o_automaton\\o_automaton_states.c")
    os.rename("..\\src\\impl_automatons\\o_automaton\\o_automaton_states.c.out", 
              "..\\src\\impl_automatons\\o_automaton\\o_automaton_states.c")
              
if len(sys.argv) == 2:
    add_new_offensive_state(sys.argv[1])
else:
    print "Wrong number of parameters"