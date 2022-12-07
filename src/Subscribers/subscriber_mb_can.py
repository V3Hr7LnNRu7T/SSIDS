import broker
import time
from src.utils.utils import type_frame
from src.Monitors.machine_observer import Observer
from src.Monitors.monitor import BA_prec_and_resp


point_connexion = broker.Endpoint()
subscriber = point_connexion.make_subscriber("/robot1/")

point_connexion.peer("127.0.0.1", 9999)


###########################################################################################################################


def main():	

    print('Press ctrl+c to exit loop and get monitors verdict \n')
    i=0

    m1 = BA_prec_and_resp('m1', "jog_mb_can", cond_x = [('fcode', broker.Count(15)), ('data', [True]), ('stadd', broker.Count(51))], cond_y =[('CobID', 0x201), ('data', 0x1f, 0, 0x1f), ('data', 0x01, 2), ('data', 0xbf, 4), ('data', 0x27, 5), ('data', 0xfd, 6), ('data', 0xff, 7)], timeinterval=[0.01, 0.5], active=True)
    m2 = BA_prec_and_resp('m2', "pp_mb_can", cond_x = [('fcode', broker.Count(15)), ('stadd', broker.Count(51)), ('data', [True])], cond_y =[('CobID', 0x201), ('data', 0x01, 0, 0x1f)], timeinterval=None, active=True)

    try:
        while True:
            if (subscriber.available()):
                i+=1
                (t,d) = subscriber.get()
                #print(d)
                trace=type_frame(d)
                m1.monitoring(trace)
                m2.monitoring(trace)


    except KeyboardInterrupt:
        pass
        
    print('frames :', i)







if __name__ == "__main__":
    main()