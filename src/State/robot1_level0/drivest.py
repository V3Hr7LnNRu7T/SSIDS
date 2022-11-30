from transitions import Machine, MachineError, EventData, State
from typing import Callable
import collections
import time
from pyIDS.utils.decorators import timer


class dvst_machine():
    def __init__(self, n):
        """
        n : node ID (servo drive 1 or 2)
        """
        self.nodeID = n
    
    def get_node(self):
        return self.nodeID

    def __repr__(self):
        return "[drive state state_machine - node {}]".format(self.nodeID)

    def process(self, frame):
        try:
            if frame.CobID == (0x180 + self.nodeID) :
                if (frame.data[0] & 0x0F) == 1:
                    self.trigger('go_to_start')
                elif (frame.data[0] & 0x0F) == 2:
                    self.trigger('go_to_not_ready_to_switch_on') 
                elif (frame.data[0] & 0x0F) == 3:
                    self.trigger('go_to_switch_on_disabled')   
                elif (frame.data[0] & 0x0F) == 4:
                    self.trigger('go_to_ready_to_switch_on')  
                elif (frame.data[0] & 0x0F) == 5:
                    self.trigger('go_to_switched_on') 
                elif (frame.data[0] & 0x0F) == 6:
                    self.trigger('go_to_operation_enabled')   
                elif (frame.data[0] & 0x0F) == 7:
                    self.trigger('go_to_quick_stop_active') 
                elif (frame.data[0] & 0x0F) == 8:
                    self.trigger('go_to_fault_reaction_active')   
                elif (frame.data[0] & 0x0F) == 9:
                    self.trigger('go_to_fault') 
                else :
                    print(time.time(), frame.data) 
                    print ('[drive state state_machine - node {}] ALERTE "unauthorized drive state value"'.format(self.nodeID))
                #print('node', self.nodeID, self.state)
        except MachineError as err:
            pass
            print(time.time())
            print('[drive state state_machine - node {}] ALERTE {}'.format(self.nodeID, err))
            print('..........Reinitializing drive state state_machine..........')
            self.to_initial_state()

    def on_exit_initial_state(self, event):
        print('Drive state state machine - node {} - initial state : {}'.format(self.nodeID, event.transition.dest))

class dvst_bus(object):

    # Define states.
    states = [
        { 'name': 'initial_state'}, #Not in the standard, but used to know initial state
        { 'name': 'start'},
        { 'name': 'not_ready_to_switch_on'},
        { 'name': 'switch_on_disabled'},
        { 'name': 'ready_to_switch_on'},
        { 'name': 'switched_on'},
        { 'name': 'operation_enabled'},
        { 'name': 'quick_stop_active'},
        { 'name': 'fault_reaction_active'},
        { 'name': 'fault'}
        ]

    # Define transitions.
    transitions = [
        {'trigger':'go_to_start', 'source':['initial_state'], 'dest':'start'},
        {'trigger':'go_to_start', 'source':['start'], 'dest': None},
        {'trigger':'go_to_not_ready_to_switch_on', 'source':['initial_state', 'start'], 'dest':'not_ready_to_switch_on'},
        {'trigger':'go_to_not_ready_to_switch_on', 'source':['not_ready_to_switch_on'], 'dest': None},
        {'trigger':'go_to_switch_on_disabled', 'source':['initial_state', 'not_ready_to_switch_on', 'ready_to_switch_on', 'switched_on', 'operation_enabled', 'quick_stop_active', 'fault'], 'dest':'switch_on_disabled'},
        {'trigger':'go_to_switch_on_disabled', 'source':['switch_on_disabled'], 'dest': None},
        {'trigger':'go_to_ready_to_switch_on', 'source':['initial_state', 'switch_on_disabled', 'switched_on', 'operation_enabled'], 'dest':'ready_to_switch_on'},
        {'trigger':'go_to_ready_to_switch_on', 'source':['ready_to_switch_on'], 'dest': None},
        {'trigger':'go_to_switched_on', 'source':['initial_state', 'ready_to_switch_on', 'operation_enabled'], 'dest':'switched_on'},
        {'trigger':'go_to_switched_on', 'source':['switched_on'], 'dest': None},
        {'trigger':'go_to_operation_enabled', 'source':['initial_state', 'switched_on', 'quick_stop_active'], 'dest':'operation_enabled'},
        {'trigger':'go_to_operation_enabled', 'source':['operation_enabled'], 'dest': None},
        {'trigger':'go_to_quick_stop_active', 'source':['initial_state', 'operation_enabled'], 'dest':'quick_stop_active'},
        {'trigger':'go_to_quick_stop_active', 'source':['quick_stop_active'], 'dest': None},
        {'trigger':'go_to_fault_reaction_active', 'source':['initial_state', 'start', 'not_ready_to_switch_on', 'switch_on_disabled', 'ready_to_switch_on', 'switched_on', 'operation_enabled', 'quick_stop_active', 'fault'], 'dest':'fault_reaction_active'},
        {'trigger':'go_to_fault_reaction_active', 'source':['fault_reaction_active'], 'dest': None},
        {'trigger':'go_to_fault', 'source':['initial_state', 'fault_reaction_active'], 'dest':'fault'},
        {'trigger':'go_to_fault', 'source':['fault'], 'dest': None}
        ]
    
    def __init__(self, nm, s1, s2):

        self.name = nm
        self.stmach1 = s1
        self.stmach2 = s2
        # Initialize the state machine
        self.machine = Machine(model=[s1, s2], states=dvst_bus.states, transitions=dvst_bus.transitions, queued=True, initial='initial_state', send_event=True)


    def on_event(self, frame):
        self.stmach1.process(frame)
        self.stmach2.process(frame)

        
    def subscribe_to_state(self, func1: Callable, func2: Callable, state: State):
        """
        func1: function to call on enter state
        func2: function to call on exit state
        state: considered state
        """
        state.on_enter.append(func1)
        state.on_exit.append(func2)

    def unsubscribe_to_state(self, func: Callable, state: State):
        state.on_enter.remove(func)