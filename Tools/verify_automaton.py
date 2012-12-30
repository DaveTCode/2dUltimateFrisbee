################################################################################
# verify_automaton                                                             #
#                                                                              #
# Tool that does basic verification checking on an automaton in order to prove #
# that it does not have any logical inconsistencies.                           #
# Check performed are as follows:                                              #
# - All xml have correct start and end tags.                                   #
# - Every automaton has a name.                                                #
# - Every automaton has a csv and lua file.                                    #
# - Every automaton set has a start automaton.                                 #
# - Every automaton has a start state.                                         #
# - Each transition has a name.                                                #
# - Each transition has a true section and a false section.                    #
# - - Both sections must have either a state, a transition or an automaton     #
#     with a state.                                                            #
# - Each transition must have a lua function name associated with it.          #
#                                                                              #
# - The lua function file must exist.                                          #
# - The lua function file must contain every function that was referred to by  #
#   the xml.                                                                   #
# - The lua functions that were called by the xml must have the correct form.  # DAT - What will this be????
#                                                                              #
# - The csv file must exist.                                                   #
# - The csv file must be properly formed with the correct number of rows/cols. #
################################################################################

import re
import sys
import os
import datetime
from xml.dom import DOMException, minidom
from xml.parsers.expat import ExpatError

DEFAULT_REPORT_FILE = "automaton_report.txt"
    
################################################################################
# Class: Logger                                                                #
#                                                                              #
# The logger class keeps track of the final report that will be provided on    #
# the automaton.                                                               #
################################################################################
class Logger:
    def __init__(self, automaton, report_file = DEFAULT_REPORT_FILE):
        self.report_file = report_file
        self.errors = []
        self.warnings = []
        self.num_automatons = 0
        self.num_transitions = 0
        self.automaton = automaton
        
    ############################################################################
    # add_error                                                                #
    #                                                                          #
    # Adds an error to the list of errors for reporting.                       #
    ############################################################################
    def add_error(self, error):
        self.errors.append(error)
        
    ############################################################################
    # add_warning                                                              #
    #                                                                          #
    # Adds a warning to the list of warnings for reporting.                    #
    ############################################################################
    def add_warning(self, warning):
        self.warnings.append(warning)
    
    ############################################################################
    # create_report                                                            #
    #                                                                          #
    # Outputs the report to file for viewing by a user.                        #
    ############################################################################
    def create_report(self):
        outfile = None
        try:
            # If this fails then there really isn't anything we can do.
            outfile = open(self.report_file, 'w')
            
            # Print the report to file.
            outfile.write('Report generated for ' + self.automaton + ' on ' + datetime.datetime.now().strftime("%Y-%m-%d %H:%M") + '\n')
            outfile.write('\n')
            outfile.write('There were ' + str(len(self.errors)) + ' errors:\n')
            for error in self.errors:
                outfile.write('\t' + error + '\n')
            
            outfile.write('\n')
            outfile.write('There were ' + str(len(self.warnings)) + ' warnings:\n')
            for warning in self.warnings:
                outfile.write('\t' + warning + '\n')
            
            outfile.write('\n')
            #outfile.write('There were ' + str(self.num_automatons) + ' automatons containing ' + str(self.num_transitions) + ' transitions\n')
        except IOError as (errno, strerror):
            print "Failure writing out report. Nothing we can do."
            print errno
            print strerror
        finally:
            if outfile != None:
                outfile.close()

################################################################################
# Class: AutomatonChecker                                                      #
#                                                                              #
# This is the main class that is called for a given automaton that we wish to  #
# verify. Once created it has one public method check_file which does all the  #
# verification and creates a report in the internal logger class.              #
#                                                                              #
# There is a further internal method output_report which is called after the   #
# verification is complete and outputs the report to the file originally req'd #
################################################################################
class AutomatonChecker:
    def __init__(self, project_dir, root_file, report_file = DEFAULT_REPORT_FILE):
        self.root_file = os.path.join(project_dir, root_file)
        self.logger = Logger(root_file, os.path.join(project_dir, report_file))
        self.project_dir = project_dir
        
    def output_report(self):
        self.logger.create_report()
        
    def _get_text(self, nodelist):
        rc = ""
        for node in nodelist:
            if node.nodeType == node.TEXT_NODE:
                rc = rc + node.data
        return rc    
     
    def check_file(self):
        xml_doc = None
        try:
            xml_doc = minidom.parse(self.root_file)
        except IOError, e:
            # Check 1: Input file exists and is readable.  
            self.logger.add_error("Catastrophic Failure: File does not exist or cannot be opened for reading.")
            return
        except ExpatError, e:
            # Check 2: XML is well formed. We test this by parsing the file and
            # catching exceptions.
            self.logger.add_error("Catastrophic Failure: XML is not well formed - line %i: column %i" %(e.lineno, e.offset))
            return
        
        # Check 3: Make sure that the top level node has the right tag name.
        automaton_set_node = xml_doc.firstChild
        if automaton_set_node.localName != "automaton_set":
            self.logger.add_error("Serious Error: Top level node should be called automaton_set and is actually %s" % automaton_set_node.localName)
        
        # Get a list of all the automaton nodes in this document (must be children of the automaton set).
        automaton_nodes = []
        start_automaton = None
        for childNode in automaton_set_node.childNodes:
            if childNode.localName == "automaton":
                automaton_nodes.append(childNode)
            elif childNode.localName == "start_automaton":
                start_automaton = self._get_text(childNode.childNodes)
            else:
                if childNode.localName != None:
                    self.logger.add_warning("Warning: Unknown node exists - " + childNode.toxml())
                
        # Check 4: Verify that there is a start automaton node.
        if start_automaton == None:
            self.logger.add_error("Serious Error: <start_automaton> tag missing from file")
        
        reqd_automaton_dict = {}
        automaton_name_list = []
        for automaton_node in automaton_nodes:
            automaton_name = ''
        
            # Check 5: Verify that there is a name attribute for each 
            if not ("name" in automaton_node.attributes.keys()):
                self.logger.add_error("Serious Error: There exists an automaton with no name attribute: " + automaton_node.toxml())
                automaton_name = 'NO NAME'
            else:
                automaton_name = automaton_node.attributes["name"].value
                automaton_name_list.append(automaton_name)
            
            lua_file = None
            csv_file = None
            transition_nodes = []
            start_state = None
            for childNode in automaton_node.childNodes:
                if childNode.localName == "lua_file":
                    lua_file = self._get_text(childNode.childNodes)
                elif childNode.localName == "csv_file":
                    csv_file = self._get_text(childNode.childNodes)
                elif childNode.localName == "transition":
                    transition_nodes.append(childNode)
                elif childNode.localName == "start_state":
                    start_state = self._get_text(childNode.childNodes)
                else:
                    if childNode.localName != None:
                        self.logger.add_warning("Warning: Unknown node exists - " + childNode.toxml())
                    
            # Check 6: Verify that lua file, csv file and start state tags exist.
            if lua_file == None:
                self.logger.add_error("Serious Error: automaton %s does not contain a lua_file tag" % automaton_name)
            if csv_file == None:
                self.logger.add_error("Serious Error: automaton %s does not contain a csv_file tag" % automaton_name)
            if start_state == None:
                self.logger.add_error("Serious Error: automaton %s does not contain a start_state tag" % automaton_name)
                
            transition_name_list = []
            lua_function_list = []
            reqd_transition_dict = {}
            for transition_node in transition_nodes:
                transition_name = 'NO NAME'
                lua_function_name = None
                
                for childNode in transition_node.childNodes:
                    if childNode.localName == "name":
                        transition_name = self._get_text(childNode.childNodes)
                        transition_name_list.append(transition_name)
                    elif childNode.localName == "lua_function_name":
                        lua_function_name = self._get_text(childNode.childNodes)
                        lua_function_list.append(lua_function_name)
                    elif childNode.localName == "true":
                        foundTrueState = False
                        foundTrueTransition = False
                        foundTrueAutomaton = False
                        for trueChildNode in childNode.childNodes:
                            if trueChildNode.localName == "state":
                                foundTrueState = True
                            elif trueChildNode.localName == "transition":
                                foundTrueTransition = True
                                reqd_transition_dict[self._get_text(trueChildNode.childNodes)] = childNode.toxml()
                            elif trueChildNode.localName == "automaton":
                                foundTrueAutomaton = True
                                reqd_automaton_dict[self._get_text(trueChildNode.childNodes)] = childNode.toxml()
                            else:
                                if trueChildNode.localName != None:
                                    self.logger.add_warning("Warning: Unknown node exists - " + trueChildNode.toxml())
                        
                        if foundTrueAutomaton and not (foundTrueState or foundTrueAutomaton):
                            self.logger.add_error("Serious Error: True automaton found for transition but no state - " + childNode.toxml())
                        elif not (foundTrueState or foundTrueTransition):
                            self.logger.add_error("Serious Error: Neither true state not transition found for transition - " + childNode.toxml())
                        elif foundTrueState and foundTrueTransition:
                            self.logger.add_warning("Warning: Both true transition and state found for transition - " + childNode.toxml())
                        
                    elif childNode.localName == "false":
                        foundFalseState = False
                        foundFalseTransition = False
                        foundFalseAutomaton = False
                        for trueChildNode in childNode.childNodes:
                            if trueChildNode.localName == "state":
                                foundFalseState = True
                            elif trueChildNode.localName == "transition":
                                foundFalseTransition = True
                                reqd_transition_dict[self._get_text(trueChildNode.childNodes)] = childNode.toxml()
                            elif trueChildNode.localName == "automaton":
                                foundFalseAutomaton = True
                                reqd_automaton_dict[self._get_text(trueChildNode.childNodes)] = childNode.toxml()
                            else:
                                if trueChildNode.localName != None:
                                    self.logger.add_warning("Warning: Unknown node exists - " + trueChildNode.toxml())
                        
                        if foundFalseAutomaton and not (foundFalseState or foundFalseTransition):
                            self.logger.add_error("Serious Error: False automaton found for transition but no state - " + childNode.toxml())
                        elif not (foundFalseState or foundFalseTransition):
                            self.logger.add_error("Serious Error: Neither false state not transition found for transition - " + childNode.toxml())
                        elif foundFalseState and foundFalseTransition:
                            self.logger.add_warning("Warning: Both false transition and state found for transition - " + childNode.toxml())
                            
                # Now that we have iterated over the whole of the transition 
                # check to make sure that the transition has a name and a lua
                # function associated with it.
                if lua_function_name == None:
                    self.logger.add_error("Serious Error: Lua function name missing from the transition - " + transition_name)
                if transition_name == 'NO NAME':
                    self.logger.add_error("Serious Error: Transition missing name tag - " + transitionNode.toxml())
                    
            # We have a dictionary containing as keys the transitions that other
            # transitions referred to (these will be in the same automaton).
            # We also have a list of all the transition names.
            # Using these two we make sure that there are no missing transitions.
            for reqd_transition_name in reqd_transition_dict.keys():
                if not reqd_transition_name in transition_name_list:
                    self.logger.add_error("Serious Error: Transition %s requested is missing - %s" % (reqd_transition_name, reqd_transition_dict[reqd_transition_name]))

            # Check that the lua file exists and that the lua functions that 
            # are referred to are contained in the lua file.
            if lua_file != None:
                self._check_lua_file(lua_file, lua_function_list)
            
            #
            if csv_file != None:
                self._check_csv_file(csv_file, transition_name_list)
        
    ############################################################################
    # _check_lua_file                                                          #
    #                                                                          #
    # Takes a list of lua function names and the file they are supposed to be  #
    # in and verifies that the file exists, can be opened and that the funcs   #
    # are there as expected.                                                   #
    ############################################################################
    def _check_lua_file(self, filename, expected_functions):
        lua_file = None
        
        try:
            lua_file = open(os.path.join(self.project_dir, filename), 'r')
            
            # Read all the lines in from the lua file. We can then search this array
            # for the function names.
            file_lines = lua_file.readlines()
            
            for function in expected_functions:
                found_function = False
                
                # This regular expression matches against the first part of the 
                # function definition in a lua file.
                function_line_regex = re.compile("\s*function\s+" + function + ".*")
                for line in file_lines:
                    if function_line_regex.match(line) != None:
                        found_function = True
                        break
                
                if not found_function:
                    self.logger.add_error("Serious Error: Lua function %s was missing from file %s" % (function, os.path.join(self.project_dir,filename)))
        except IOError, e:
            self.logger.add_error("Serious error: lua file %s does not exist or could not be opened %s" % (os.path.join(self.project_dir,filename), str(e)))
            return
        finally:
            if lua_file != None:
                lua_file.close()
                
    ############################################################################
    # _check_csv_file                                                          #
    #                                                                          #
    # Takes a csv filename and the list of available transitions and checks    #
    # that all of the transitions in the csv file correspond to one in the     #
    # list.                                                                    #
    ############################################################################
    def _check_csv_file(self, filename, transitions):
        # Attempt to open the csv file and log an error if this fails.
        csv_file = None
        
        try:
            csv_file = open(os.path.join(self.project_dir, filename), 'r')
            
            first_line = True
            num_events = 0
            for line in csv_file:
                if first_line:
                    first_line = False
                    num_events = line.count(',')
                else:
                    if num_events != line.count(','):
                        self.logger.add_error("Serious Error: Csv file %s contains line %s which has %i cells and should have %i" % (os.path.join(self.project_dir,filename), line, line.count(','), num_events))
                    
                    (state, all_transitions) = line.split(',', 1)
                    trans_list = all_transitions.split(',', num_events - 1)
                    for transition in trans_list:
                        transition = transition.strip("\t\n ")
                        if transition != '' and not transition in transitions:
                            self.logger.add_error("Serious Error: Csv file %s contains line %s which has a transition %s that is not in the xml" % (os.path.join(self.project_dir,filename), line, transition))
        except IOError, e:
            self.logger.add_error("Serious error: csv file %s does not exist or could not be opened %s" % (os.path.join(self.project_dir,filename), str(e)))
            return
        finally:
            if csv_file != None:
                csv_file.close()
        
################################################################################
# This file is called with 1 parameter: The xml transition file that we want   #
# to verify.                                                                   #
################################################################################
if __name__ == "__main__":
    if len(sys.argv) == 4:
        ac = AutomatonChecker(sys.argv[1], sys.argv[2], sys.argv[3])
        ac.check_file()
        ac.output_report()
    else:
        print "Correct usage: python verify_automaton.py <project_dir> <automaton_transition_file.xml> <output_file>"