# trames.py

class Trame(object) :
    """ 
    Trames CAN+mbrtu
    """

    def __init__(self, d):
        self.protocole = d[2][0]
        self.data = [] 
        self.ts = None
        self.CobID = None
        self.DLC = None
        self.nodeID = None
        self.fcode = None
        self.stadd = None
        self.bc = None


    def selector(self, field, val, index = None, logical_and = None):
        """
        compare frame field and value - return T or F
        node(opt): val+nodeID
        index(opt): field index
        logical_and(opt):applied to field
        
        examples:
        selector('data', 0x06, 0, 0x1F) return "data[0]&0x1F == 0x06"
        selector('CobID', 0x201) return "CobID == 0x201"
        selector('CobID', 0x200, True) return "CobID == 0x200+nodeID"
        """

        # if node:
        #     val += self.nodeID

        if (index != None):
            if (logical_and != None):
                return ((getattr(self, field)[index] & logical_and) == val)
            else:
                return (getattr(self, field)[index] == val)
        else:
            return (getattr(self, field) == val)


class trame_can(Trame):

    def __init__(self, d):
        super().__init__(d)
        self.ts = d[2][1][0]
        self.CobID = d[2][1][1]
        self.DLC = d[2][1][2]
        self.nodeID = self.CobID & 0x7F
        for i in range(len(d[2][1][6])):
            self.data.append(d[2][1][6][i])

#########################################################

class trame_mbtcp(Trame):

    def __init__(self, d):
        super().__init__(d)
        self.ts = d[2][1][0][32][0]
        self.fcode = d[2][1][1][3]

class trame_mbtcp_wmcr(trame_mbtcp):

    def __init__(self, d):
        super().__init__(d)
        self.stadd = d[2][1][2]
        for i in range(len(d[2][1][3])):
            self.data.append(d[2][1][3][i])
        self.bc = len(self.data)






class trame_attaque(Trame):
    def __init__(self, d):
        super().__init__(d)
        self.fcode = d[2][1][1][3]
