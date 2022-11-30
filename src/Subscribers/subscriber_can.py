import broker
import time
from pyIDS.utils.utils import type_trame
from pyIDS.Monitors.machine_observer import Observer
from pyIDS.State.robot1_level0.canstmach import nmt_bus, nmt_machine
from pyIDS.State.robot1_level0.drivest import dvst_machine, dvst_bus
from pyIDS.State.robot1_level0.drivemd import dvmd_machine, dvmd_bus
from pyIDS.State.robot1_level0.sp import speed_and_pos, speed_and_pos_bus
from pyIDS.State.robot1_level0.homing_valid import homing_machine, homing_bus
from pyIDS.Monitors.monitor import never



# Definir un point de connexion ("endpoint") et s'abonner a  "trafic" (CAN+mbtcp)
point_connexion = broker.Endpoint()
subscriber = point_connexion.make_subscriber("/robot1/")

#se connecter au serveur
point_connexion.peer("127.0.0.1", 9999)


###########################################################################################################################




def main():	
    print('Press ctrl+c to exit loop and get monitors verdict \n')

    nmt_machines, dvst_machines, dvmd_machines, homing_machines = state_machines_instantiation()

    sp1 = speed_and_pos(1, 3)
    sp2 = speed_and_pos(2, 3)
    sp_memory = speed_and_pos_bus('s&p', sp1, sp2)


    i=0
    monitors = monitors_instantiation(nmt_machines, dvst_machines, dvmd_machines, homing_machines)

    try:
        while True:
            if (subscriber.available()):
                i+=1
                (t,d) = subscriber.get()
                trace=type_trame(d)

                nmt_machines.on_event(trace)
                dvst_machines.on_event(trace)
                dvmd_machines.on_event(trace)
                #sp_memory.on_event(trace)
                homing_machines.on_event(trace)

                for m in monitors:
                    m.monitoring(trace)



    except KeyboardInterrupt:
        pass
        
    print('frames :', i)
    






def state_machines_instantiation():
    print('.....................State machines instantiation.....................')
    m1 = nmt_machine(1, 3)
    m2 = nmt_machine(2, 3)
    nmt_machines = nmt_bus('nmt', m1, m2)

    m3 = dvst_machine(1)
    m4 = dvst_machine(2)
    dvst_machines = dvst_bus('dvst', m3, m4)

    m5 = dvmd_machine(1)
    m6 = dvmd_machine(2)
    dvmd_machines = dvmd_bus('dvmd', m5, m6)

    h1 = homing_machine(1)
    h2 = homing_machine(2)
    homing_machines = homing_bus('homing', h1, h2)
 
    return (nmt_machines, dvst_machines, dvmd_machines, homing_machines)



def monitors_instantiation(nmt_machines, dvst_machines, dvmd_machines, homing_machines):
    print('.....................Scope Recognizer and monitors instantiation..................... \n ')

    obs = Observer() #An observer subscribes to states, in order to activate monitors
    #print(nmt_machines.machine.get_state(m1.state))
     
    nmt_machines.subscribe_to_state(obs.nmt_pre_op_activate, obs.nmt_pre_op_deactivate, nmt_machines.machine.get_state('pre_operational'))
    nmt_machines.subscribe_to_state(obs.nmt_op_activate, obs.nmt_op_deactivate, nmt_machines.machine.get_state('operational'))
    nmt_machines.subscribe_to_state(obs.nmt_st_activate, obs.nmt_st_deactivate, nmt_machines.machine.get_state('stopped'))

    dvst_machines.subscribe_to_state(obs.drivest_rdso_activate, obs.drivest_rdso_deactivate, dvst_machines.machine.get_state('ready_to_switch_on'))

    dvmd_machines.subscribe_to_state(obs.drivemd_no_mvt_activate, obs.drivemd_no_mvt_deactivate, dvmd_machines.machine.get_state('no_movement'))
    dvmd_machines.subscribe_to_state(obs.drivemd_pp_activate, obs.drivemd_pp_deactivate, dvmd_machines.machine.get_state('profile_position'))
    dvmd_machines.subscribe_to_state(obs.drivemd_hom_activate, obs.drivemd_hom_deactivate, dvmd_machines.machine.get_state('homing'))
    dvmd_machines.subscribe_to_state(obs.drivemd_jog_activate, obs.drivemd_jog_deactivate, dvmd_machines.machine.get_state('jog')) 

    homing_machines.subscribe_to_state(obs.homing_valid_activate, obs.homing_valid_deactivate, homing_machines.machine.get_state('homing_valid'))
    homing_machines.subscribe_to_state(obs.homing_not_valid_activate, obs.homing_not_valid_deactivate, homing_machines.machine.get_state('homing_not_valid'))
 
    return obs.get_list_of_monitors()



if __name__ == "__main__":
    main()