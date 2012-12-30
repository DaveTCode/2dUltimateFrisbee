import os

input_file = open("o_pull_automaton/o_pull_automaton_names.h", "r")
csv_file = open("o_pull_automaton/o_pull_automaton.csv", "w")

states = []
num_states = 0
event_line = ""

for line in input_file:
    if line.startswith("#define O_PULL_AUTOMATON_EVENT_"):
        event_name = line[line.index('"') + 1: line.rindex('"')]
        event_line = event_line + ", " + event_name
        num_states = num_states + 1
        
    if line.startswith("#define O_PULL_AUTOMATON_STATE_"):
        state_name = line[line.index('"') + 1: line.rindex('"')]
        states.append(state_name)
        
csv_file.write(event_line + '\n')
for state_name in states:
    csv_file.write(state_name + "," * num_states + '\n')