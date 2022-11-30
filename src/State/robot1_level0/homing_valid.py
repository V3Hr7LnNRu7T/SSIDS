from transitions import Machine, MachineError, EventData, State
from typing import Callable
import collections
import time
from pyIDS.utils.decorators import timer


class homing_machine():
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
        return "[homing valid state_machine - node {}]".format(self.nodeID)


    def process(self, frame):
        try:
            if frame.CobID == (0x180 + self.nodeID) :
                if (frame.data[1] & 0x02) == 0x02:
                    self.trigger('go_to_homing_valid')
                elif (frame.data[1] & 0x02) == 0x00:
                    self.trigger('go_to_homing_not_valid')
                else :
                    #print(time.time())
                    print ('[homing valid state_machine - node {}] ALERTE "unauthorized transition"'.format(self.nodeID))                   

                #print('node', self.nodeID, self.state)
        except MachineError as err:
            pass
            #print(time.time())
            print('[homing valid state_machine - node {}] ALERTE {}'.format(self.nodeID, err))
            print('..........Reinitializing homing valid state_machine..........')
            self.to_initial_state()
    


class homing_bus(object):

    # Define states.
    states = [
        { 'name': 'initial_state'},
        { 'name': 'homing_valid'},
        { 'name': 'homing_not_valid'},
        ]

    # Define transitions.
    transitions = [
        {'trigger':'go_to_homing_valid', 'source':['initial_state', 'homing_not_valid'], 'dest': 'homing_valid'},
        {'trigger':'go_to_homing_valid', 'source':['homing_valid'], 'dest': None},
        {'trigger':'go_to_homing_not_valid', 'source':['initial_state'], 'dest': 'homing_not_valid'},
        {'trigger':'go_to_homing_not_valid', 'source':['homing_not_valid'], 'dest': None},
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
        self.machine = Machine(model=[s1, s2], states=homing_bus.states, transitions=homing_bus.transitions, queued=True, initial='initial_state', send_event=True)


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
