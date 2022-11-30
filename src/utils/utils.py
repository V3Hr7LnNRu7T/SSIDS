from pyIDS.utils.trames import *

def type_trame(res):

    
    if res[2][0] == 'good_can_request_1': 
        return trame_can(res)
    if res[2][0] == 'modbus_message' :
        try:
            return trame_mbtcp(res)
        except:
            return trame_attaque(res)
    if res[2][0] == 'modbus_write_multiple_coils_request' :
        return trame_mbtcp_wmcr(res)
    else:
        return Trame(res)

