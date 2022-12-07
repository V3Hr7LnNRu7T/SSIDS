from src.utils.frames import *

def type_frame(res):

    
    if res[2][0] == 'good_can_request_1': 
        return can_frame(res)
    if res[2][0] == 'modbus_message' :
        return mbtcp_frame(res)
    if res[2][0] == 'modbus_write_multiple_coils_request' :
        return mbtcp_wmcr_frame(res)
    else:
        return frame(res)

