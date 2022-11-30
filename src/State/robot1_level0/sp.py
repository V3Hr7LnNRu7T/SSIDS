import collections

class speed_and_pos():
    def __init__(self, n, history_length):
        """
        n : node ID (servo drive 1 or 2)
        history_length : number of previous state to save
        """
        self.nodeID = n
        self.position_history = collections.deque(maxlen=history_length)
        self.speed_history = collections.deque(maxlen=history_length)
        self.borne_inf = {1 : [0x00, 0x04, 0x90, 0x00], 2 : [0x00, 0x03, 0xf0, 0x00]}
        self.borne_supborne_sup = {1 : [0xff, 0xff, 0xf0, 0x00], 2 : [0xff, 0xff, 0xf0, 0x00]}        

    def get_node(self):
        return self.nodeID

    def process(self, frame):
        if frame.CobID == (0x280 + self.nodeID) :
                self.position=frame.data
                self.speed=frame.data
                #print('node {} current position : {}'.format(self.nodeID, self.position))
                #print('node {} current speed : {}'.format(self.nodeID, self.speed))
                #print('node {} last positions : {}'.format(self.nodeID, "--->".join(map(str,self.position_history))))
                #print('node {} last speeds : {}'.format(self.nodeID, "--->".join(map(str, self.speed_history))))
                
    @property
    def position(self):
        return self.position_history[-1]
    
    @property
    def speed(self):
        return self.speed_history[-1]

    @position.setter
    def position(self, value):
        res = value[:4]
        res.reverse()
        self.position_history.append(res)

    @speed.setter
    def speed(self, value):
        res = value[4:]
        res.reverse()
        self.speed_history.append(res)
    

class speed_and_pos_bus(object):
    
    def __init__(self, nm, s1, s2):
        """
        nm : name of the 'bus machine'
        s1 : first model of the bus machine -> corresponds to the first network node (servo drive 1)
        s2 : second model -> second network node (servo drive 2)
        """
        
        self.name = nm
        self.stmach1 = s1
        self.stmach2 = s2

    def on_event(self, frame):
        self.stmach1.process(frame)
        self.stmach2.process(frame)