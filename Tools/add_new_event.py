import os
import re
import sys

################################################################################
# add_new_event                                                                #
#                                                                              #
# Adds a new event to the game.                                                #
#                                                                              #
# Parameters: event - String name of the event.                                #
################################################################################
def add_new_event(event):
    event_name_file_in = open("..\\resources\\automaton\\event_names.txt", "r")
    event_header_file_in = open("..\\src\\impl_automatons\\generic_o_d_files\\event_names.h","r")
    event_header_file_out = open("..\\src\\impl_automatons\\generic_o_d_files\\event_names.h.out","w")
    event_code_file_in = open("..\\src\\impl_automatons\\generic_o_d_files\\init_automaton_events.c","r")
    event_code_file_out = open("..\\src\\impl_automatons\\generic_o_d_files\\init_automaton_events.c.out","w")
    
    found_event_line = False
    for line in event_name_file_in:
        if line == event + "\n":
            found_event_line = True
            break
    event_name_file_in.close()
    
    if not found_event_line:
        event_name_file_out = open("..\\resources\\automaton\\event_names.txt", "a")
        event_name_file_out.write(event + "\n")
        event_name_file_out.close()
    
    header_line_written = False
    for line in event_header_file_in:
        if line.find("NUM_AUTOMATON_EVENTS") != -1:
            num_events_match = re.search("[0-9]+", line)
            num_events = int(num_events_match.group(0)) + 1
            output_line = "#define NUM_AUTOMATON_EVENTS " + str(num_events) + "\n"
        elif line.find("AUTOMATON_EVENT_") != -1 and header_line_written == False:
            output_line = "#define AUTOMATON_EVENT_" + event.upper() + ' "event_' + event + '"\n'
            output_line = output_line + line
            header_line_written = True
        else:
            output_line = line
            
        event_header_file_out.write(output_line)
        
    event_header_file_in.close()
    event_header_file_out.close()
    os.remove("..\\src\\impl_automatons\\generic_o_d_files\\event_names.h")
    os.rename("..\\src\\impl_automatons\\generic_o_d_files\\event_names.h.out", 
              "..\\src\\impl_automatons\\generic_o_d_files\\event_names.h")
        
    for line in event_code_file_in:
        if line.find("default:") != -1:
            output_line = "      case " + str(num_events - 1) + ":\n"
            output_line = output_line + "        strncpy((*event_array)[ii]->name,\n"
            output_line = output_line + "                AUTOMATON_EVENT_" + event.upper() + ",\n"
            output_line = output_line + "                MAX_EVENT_NAME_LEN);\n"
            output_line = output_line + "        break;\n"
            output_line = output_line + line
        else:
            output_line = line
            
        event_code_file_out.write(output_line)
        
    event_code_file_in.close()
    event_code_file_out.close()
    os.remove("..\\src\\impl_automatons\\generic_o_d_files\\init_automaton_events.c")
    os.rename("..\\src\\impl_automatons\\generic_o_d_files\\init_automaton_events.c.out", 
              "..\\src\\impl_automatons\\generic_o_d_files\\init_automaton_events.c")
              
if len(sys.argv) == 2:
    add_new_event(sys.argv[1])
else:
    print "Wrong number of parameters"