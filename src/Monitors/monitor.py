from transitions import Machine
from typing import Callable
import time
from datetime import timedelta
import broker


class monitor(object):
    """
    """
    
    def __init__(self, name, description, active = False):
        self.name = name
        self.desc = description     
        self.active = active
        self.alert = False
        
    def activate(self, n=None):
        self.considered_node = n
        self.active = True

    def deactivate(self):
        self.active = False
    
    def print_alert(self, frame = None):
        self.alert = True
        if frame!=None:
            self.considered_node=frame.nodeID
        print(time.time())
        try:
            print('[monitor {} - node {}] ALERTE "{}"'.format(self.name, self.considered_node, self.desc))
        except:
            print('[monitor {}] ALERTE "{}"'.format(self.name, self.desc))
        
        
    def cond_true(self, var, frame):
        try:
            l=[]
            for cond in var:
                try:
                    res = frame.selector(*cond)
                except:
                    res=False
                l.append(res)
            return(all(l)) 
        except:
            return(frame.selector(*var))




###################################################

class never(monitor):

    def __init__(self, name, description, condition, active = False):
        super().__init__(name, description, active = active)
        self.e = condition

        
    def monitoring(self, frame):
        if self.active==True:
            if (frame.fcode==broker.Count(5)):
                self.print_alert(frame)
            if self.cond_true(self.e, frame):
                self.print_alert(frame)



class whitelist(monitor):

    def __init__(self, name, description, condition, active = False):
        super().__init__(name, description, active = active)
        self.e = condition
        self.considered_node = None
     
    def monitoring(self, frame):
        if self.active==True:
            if frame.nodeID == self.considered_node:
                if self.cond_true(self.e, frame) == False:
                    self.print_alert(frame)
    






class BA_prec_and_resp(monitor):

    # Define states.
    states = [
        { 'name': '0', 'on_exit': ['prop_unsatisfied'], 'on_enter': ['prop_satisfied']},
        { 'name': '1', 'on_exit': ['stop_clock'], 'on_enter': ['start_clock']},
        { 'name': 'forbidden', 'on_enter': ['print_alert']}
        ]

    # Define transitions.
    # 'prepare' callback is executed as soon as a transition starts, before any 'conditions' are checked or other callbacks are executed.
    transitions = [
        {'trigger':'x', 'source':'0', 'dest':'1'},
        {'trigger':'y', 'source':'0', 'dest':'forbidden'},
        {'trigger':'x', 'source':'1', 'dest':'forbidden'},
        {'trigger':'y', 'source':'1', 'dest':'0'},
        {'trigger':'x', 'source':'forbidden', 'dest': None},
        {'trigger':'y', 'source':'forbidden', 'dest': '='}, #si je veux afficher des alertes a chaque fois que je rentre dans l'etat f, mettre '=' a la place de None
        ]

    def __init__(self, name, description, cond_x, cond_y, timeinterval=None, active=False):
    #def __init__(self, *args, cond_x, cond_y, timeinterval = None):
        
        super().__init__(name, description, active = active)
        
        # Initialize the state machine
        self.machine = Machine(model=self, states=BA_prec_and_resp.states, transitions=BA_prec_and_resp.transitions, queued=True, initial='0')
        # Mapping
        self.cond_transition_x = cond_x
        self.cond_transition_y = cond_y
        # Interval autorise en ms
        self.inter = timeinterval
        # State of security pattern to monitor
        self.is_prop_satisfied = True
        # For timed transitions
        self.ts = None
        self.start_time = None


    def monitoring(self, frame):
        # if (self.ts is not None):
        #     print('delta ts', (getattr(frame, 'ts')-self.ts)/timedelta(milliseconds=1))
        #     print('delta temps', (time.time()-self.tempo)*1000)
        # self.tempo = time.time()
        self.ts = getattr(frame, 'ts')
        if self.cond_true(self.cond_transition_x, frame):
            #print('transition x', self.ts)
            self.trigger('x')
        if self.cond_true(self.cond_transition_y, frame):
            #print('transition y', self.ts)
            self.trigger('y')

        if (self.start_time is not None and self.inter is not None):
            if (((self.ts - self.start_time)/timedelta(milliseconds=1))>self.inter[1]):
                print((self.ts - self.start_time)/timedelta(milliseconds=1))
                print('no response - go to forbidden state')
                self.to_forbidden()        
    

    def deactivate(self):
        self.active = False
        if self.verdict() == False:
            self.print_alert()
        self.to_0()

    def prop_unsatisfied(self):
        self.is_prop_satisfied = False
    
    def prop_satisfied(self):
        self.is_prop_satisfied = True
    
    def verdict(self):
        return self.is_prop_satisfied

    @property
    def start_clock(self):
        self.start_time = self.ts
        #print('start clock . ts = ', self.start_time)

    def stop_clock(self):
        if self.inter is not None:
            if ((self.ts-self.start_time)/timedelta(milliseconds=1))<self.inter[0]:
                #print('stop clock', (self.ts-self.start_time)/timedelta(milliseconds=1), 'ms')
                print('response not in interval - go to forbidden state')
                self.to_forbidden()
            else:
                #print('stop clock', (self.ts-self.start_time)/timedelta(milliseconds=1), 'ms')
                self.start_time = None





