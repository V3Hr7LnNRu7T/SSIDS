from transitions import Machine, MachineError, EventData, State
from typing import Callable
import collections
import time
from src.utils.decorators import timer, run_once, repeat


class nmt_machine(Machine):
    def __init__(self, n, history_length):
        """
        n : node ID (servo drive 1 or 2)
        history_length : number of previous state to save
        """
        self.nodeID = n
        self.state_history = collections.deque(maxlen=history_length)
        self._subscriptions=[]

    def __repr__(self):
        return "[nmt state_machine - node {}]".format(self.nodeID)
    
    def get_node(self):
        return self.nodeID

    #@timer
    def process(self, frame):
        try:
            if frame.CobID == (0x700 + self.nodeID) :
                #print('node', self.nodeID, self.state)
                #print('node {} last states : {}'.format(self.nodeID, "--->".join(self.state_history)))
                if frame.data[0] == 0:
                    self.trigger('go_to_init')
                elif frame.data[0] == 4:
                    self.trigger('go_to_stop') 
                elif frame.data[0] == 5:
                    self.trigger('go_to_op')   
                elif frame.data[0] == 127:
                    self.trigger('go_to_pre')    
                else :
                    print ('[drive mode state_machine - node {}] ALERTE "unauthorized heartbeat value"'.format(self.nodeID))
                    #print(time.time())
        except MachineError as err:
            pass
            #print(time.time())
            print('[nmt state_machine - node {}] ALERTE {}'.format(self.nodeID, err))
            #log alert - forbidden transition
            print('..........Reinitializing nmt state_machine..........')
            self.to_initial_state()

    @property
    def state(self):
        return self.state_history[-1]

    @state.setter
    def state(self, value):
        self.state_history.append(value)

    def on_exit_initial_state(self, event: EventData):
        print('CAN state machine - node {} - initial state : {}'.format(self.nodeID, event.transition.dest))


  




class nmt_bus(object):

    # Define states.
    states = [
        { 'name': 'initial_state'}, #Not in the standard but permits to start the state machine anywhere
        { 'name': 'initializing'},
        { 'name': 'pre_operational'},
        { 'name': 'operational'},
        { 'name': 'stopped'}
        ]

    # Define transitions.
    transitions = [
        {'trigger':'go_to_init', 'source':['initial_state', 'pre_operational', 'operational', 'stopped'], 'dest':'initializing'},
        {'trigger':'go_to_init', 'source':['initializing'], 'dest': None},
        {'trigger':'go_to_pre', 'source':['initial_state', 'initializing', 'operational', 'stopped'], 'dest':'pre_operational'},
        {'trigger':'go_to_pre', 'source':['pre_operational'], 'dest': None},
        {'trigger':'go_to_op', 'source':['initial_state', 'pre_operational', 'stopped'], 'dest':'operational'},
        {'trigger':'go_to_op', 'source':['operational'], 'dest': None},
        {'trigger':'go_to_stop', 'source':['initial_state', 'pre_operational', 'operational'], 'dest':'stopped'},
        {'trigger':'go_to_stop', 'source':['stopped'], 'dest': None}
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
        self.machine = Machine(model=[s1, s2], states=nmt_bus.states, transitions=nmt_bus.transitions, queued=True, initial='initial_state', send_event=True)

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








