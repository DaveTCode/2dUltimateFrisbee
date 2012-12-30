import string

statefile = open("ai_general\\ai_player_states.h", "r")
eventfile = open("ai_general\\ai_events.h", "r")
outfile = open("ai_scripts\\blank_ai.lua", "w")

found_first_state = False

for line in statefile:
    if string.find(line, "}") != -1:
        found_first_state = False

    if found_first_state and string.find(line, "{") == -1 and string.find(line, "state_invalid") == -1:
        state = string.strip(line)[:-1]
        outfile.write("--------------------------------------------------------------------------------\n")
        outfile.write("-- process_" + state + "\n")
        outfile.write("--------------------------------------------------------------------------------\n")
        outfile.write("function process_" + state + "\n")
        outfile.write("\n")
        outfile.write("end\n")
        outfile.write("\n")

    if string.find(line, "typedef enum player_state") != -1:
        found_first_state = True
        
outfile.close()
eventfile.close()
statefile.close()