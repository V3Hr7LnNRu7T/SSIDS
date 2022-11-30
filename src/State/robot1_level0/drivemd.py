from transitions import Machine, MachineError, EventData, State
from typing import Callable
import collections
import time
from pyIDS.utils.decorators import timer


class dvmd_machine():
    def __init__(self, n):
        """
        n : node ID (servo drive 1 or 2)
        history_length : number of previous state to save
        """
        self.nodeID = n
        #self.state_history = collections.deque(maxlen=history_length)
    
    def get_node(self):
        return self.nodeID

    def __repr__(self):
        return "[drive mode state_machine - node {}]".format(self.nodeID)


    def process(self, frame):
        try:
            if frame.CobID == (0x180 + self.nodeID) :
                if (frame.data[1] & 0xC0) != 0x0: #operating mode terminated with or without error
                    self.trigger('go_to_no_movement')
                elif (frame.data[2] & 0x1F) == 0x1:
                    self.trigger('go_to_profile_position')                 
                elif (frame.data[2] & 0x1F) == 0x6:
                    self.trigger('go_to_homing') 
                elif (frame.data[2] & 0x1F) == 0x1F:
                    self.trigger('go_to_jog')  
                else :
                    #print(time.time())
                    print ('[drive mode state_machine - node {}] ALERTE "unauthorized drive mode value"'.format(self.nodeID))
                
                print('node', self.nodeID, self.state)
        except MachineError as err:
            pass
            print(time.time())
            print('[drive mode state_machine - node {}] ALERTE {}'.format(self.nodeID, err))
            print('..........Reinitializing drive mode state_machine..........')
            self.to_initial_state()
    
    def on_exit_initial_state(self, event):
         print('Drive mode machine - node {} - initial state : {}'.format(self.nodeID, event.transition.dest))


class dvmd_bus(object):

    # Define states.
    states = [
        { 'name': 'initial_state'},
        { 'name': 'no_movement'},
        { 'name': 'profile_position'},
        { 'name': 'homing'},
        { 'name': 'jog'}
        ]

    # Define transitions.
    transitions = [
        {'trigger':'go_to_no_movement', 'source':['initial_state', 'profile_position', 'homing', 'jog'], 'dest': 'no_movement'},
        {'trigger':'go_to_no_movement', 'source':['no_movement'], 'dest': None},
        {'trigger':'go_to_profile_position', 'source':['initial_state', 'no_movement'], 'dest': 'profile_position'},
        {'trigger':'go_to_profile_position', 'source':['profile_position'], 'dest': None},
        {'trigger':'go_to_homing', 'source':['initial_state', 'no_movement'], 'dest': 'homing'},
        {'trigger':'go_to_homing', 'source':['homing'], 'dest': None},
        {'trigger':'go_to_jog', 'source':['initial_state', 'no_movement'], 'dest': 'jog'},
        {'trigger':'go_to_jog', 'source':['jog'], 'dest': None},
        ]
    
    def __init__(self, nm, s1, s2):
        """
        nm : name of the 'bus machine'
        s1 : first model of the bus machine -> corresponds to the first network node (servo drive 1)
        s2 : second model -> second network node (servo drive 2)
        """
        self.name = nm
        self.stmach1 = s1
        self.stmach2 = s2
        # Initialize the state machine
        self.machine = Machine(model=[s1, s2], states=dvmd_bus.states, transitions=dvmd_bus.transitions, queued=True, initial='initial_state', send_event=True)


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
