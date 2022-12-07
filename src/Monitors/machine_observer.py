import broker
from transitions import EventData
from src.Monitors.monitor import never, whitelist, BA_prec_and_resp


class Observer:
    def __init__(self):
        
        self.m1=whitelist('m1', 'Unauthorized CobID during node 1 stopped NMT state', ('CobID', 0x701))
        self.m2=never('m2', 'Unauthorized CobID during node 2 ready_to_switch_on drive state', ('CobID', 0x202), active=False)
        self.m3=never('m3', 'New movement command during movement', ('CobID', 0x201), active=False)   
        self.m4=never('m4', 'Homing command when homing already valid', (('CobID', 0x201), ('data', 0x06, 0, 0x1f)), active=False)
        self.m5 = BA_prec_and_resp('m5','illegal sequence in homing_x_y', cond_x = [('CobID', 0x201), ('data', 0x06, 0, 0x1F)] , cond_y =[('CobID', 0x202), ('data', 0x06, 0, 0x1F)], timeinterval=[0, 0.002], active=True)


        #list of monitors
        self.list_mon = []
        self.list_mon.extend((self.m1, self.m2, self.m3, self.m4, self.m5))
        

    def get_list_of_monitors(self):
        return self.list_mon


    def specific_state(self, event_data: EventData):
        print('{} - state is now {}'.format(event_data.model, event_data.state.name))

#################### NMT state machine #################### 

    def nmt_pre_op_activate(self, event_data: EventData):
        #print('{} - state is now {}'.format(event_data.model, event_data.state.name))
        if (event_data.model.nodeID == 1):
            self.m1.activate()
            self.m3.activate()
            self.m5.activate()

        if (event_data.model.nodeID == 2):
            self.m2.activate()
            self.m4.activate()

    def nmt_pre_op_deactivate(self, event_data: EventData):
        if (event_data.model.nodeID == 1):
            self.m1.deactivate()
            self.m3.deactivate()
            self.m5.deactivate()

        if (event_data.model.nodeID == 2):
            self.m2.deactivate()
            self.m4.deactivate()



    def nmt_op_activate(self, event_data: EventData):
        pass

    def nmt_op_deactivate(self, event_data: EventData):
        pass



    def nmt_st_activate(self, event_data: EventData):
        if (event_data.model.nodeID == 1):
            self.m1.activate(1)
        if (event_data.model.nodeID == 2):
            pass

    def nmt_st_deactivate(self, event_data: EventData):
        if (event_data.model.nodeID == 1):
            self.m1.deactivate()
        if (event_data.model.nodeID == 2):
            pass





#################### drivest state machine #################### 


    def drivest_rdso_activate(self, event_data: EventData):
        if (event_data.model.nodeID == 2):
            self.m2.activate(2)

    def drivest_rdso_deactivate(self, event_data: EventData):
        if (event_data.model.nodeID == 2):
            self.m2.deactivate()


#################### drivemd state machine #################### 




    def drivemd_pp_activate(self, event_data: EventData):
        if (event_data.model.nodeID == 1):
            self.m3.activate()
        if (event_data.model.nodeID == 2):
            pass

    def drivemd_pp_deactivate(self, event_data: EventData):
        if (event_data.model.nodeID == 1):
            self.m3.deactivate()
        if (event_data.model.nodeID == 2 ):
            pass



    def drivemd_hom_activate(self, event_data: EventData):
        if (event_data.model.nodeID == 1):
            self.m3.activate(1)
        if (event_data.model.nodeID == 2):
            pass

    def drivemd_hom_deactivate(self, event_data: EventData):
        if (event_data.model.nodeID == 1):
            self.m3.deactivate()
        if (event_data.model.nodeID == 2):
            pass


    def drivemd_jog_activate(self, event_data: EventData):
        if (event_data.model.nodeID == 1):
            pass
        if (event_data.model.nodeID == 2):
            pass


    def drivemd_jog_deactivate(self, event_data: EventData):
        if (event_data.model.nodeID == 1):
            pass
        if (event_data.model.nodeID == 2):
            pass
 



 #################### homing valid state machine ####################

    def homing_valid_activate(self, event_data: EventData):
        if (event_data.model.nodeID == 1):
            self.m4.activate()

    def homing_valid_deactivate(self, event_data: EventData):
        if (event_data.model.nodeID == 1):
            self.m4.deactivate()

