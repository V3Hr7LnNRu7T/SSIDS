### WORKER CAN + WORKER MBTCP ###

redef exit_only_after_terminate = T;


##
global allowed : table[count] of set[count] = 
	{
		[0x000] = set(2),
		[0x001] = set(0, 1),
		[0x07F] = set(0, 1),
		[0x080] = set(0, 1),
		[0x081] = set(8),
		[0x082] = set(8),
		[0x100] = set(6),
		[0x181] = set(0, 1, 2, 3, 4, 5, 6, 7, 8),
		[0x182] = set(0, 1, 2, 3, 4, 5, 6, 7, 8),
		[0x201] = set(0, 1, 2, 3, 4, 5, 6, 7, 8),
		[0x202] = set(0, 1, 2, 3, 4, 5, 6, 7, 8),
		[0x281] = set(0, 1, 2, 3, 4, 5, 6, 7, 8),
		[0x282] = set(0, 1, 2, 3, 4, 5, 6, 7, 8),
		[0x301] = set(0, 1, 2, 3, 4, 5, 6, 7, 8),
		[0x302] = set(0, 1, 2, 3, 4, 5, 6, 7, 8),
		[0x581] = set(8),
		[0x582] = set(8),
		[0x601] = set(8),
		[0x602] = set(8),
		[0x701] = set(1),
		[0x702] = set(1),
		[0x77F] = set(1),
	};


type int_CAN_payload : record
{
b0: int; b1: int; b2: int; b3: int; b4: int; b5: int; b6: int; b7: int;
};

global good_can_request_1: event(ts : time, CANID: int, DLC : int, RTR: int, Error: int, Extended: int, payload: int_CAN_payload);
global good_can_request_2: event(ts : time, CANID: int, DLC : int, RTR: int, Error: int, Extended: int, payload: int_CAN_payload);




event zeek_init()
	{
	Broker::peer("127.0.0.1", 9999/tcp);
    Broker::auto_publish("/robot1/can", good_can_request_1);
	Broker::auto_publish("/robot1/mbrtu", modbus_message);
	Broker::auto_publish("/robot1/mbrtu_wmcr", modbus_write_multiple_coils_request);
	}




event can_request(CANID: count, DLC : count, RTR: count, Error: count, Extended: count, payload: CAN_payload)
	{	
	if (CANID !in allowed || DLC !in allowed[CANID])
		{
		print "bad structure CAN";
		}
	else
		local p: int_CAN_payload;
		p$b0 = payload$b0;
		p$b1 = payload$b1;
		p$b2 = payload$b2;
		p$b3 = payload$b3;
		p$b4 = payload$b4;
		p$b5 = payload$b5;
		p$b6 = payload$b6;
		p$b7 = payload$b7;
		event good_can_request_1(network_time(), CANID, DLC, RTR, Error, Extended, p);
		# {	
	    # if (Cluster::node=="worker-can1")
		# 	{
		# 	event good_can_request_1(network_time(), CANID, DLC, RTR, Error, Extended, p);
		# 	}
		# if (Cluster::node=="worker-can2")
		# 	{
		# 	event good_can_request_2(network_time(), CANID, DLC, RTR, Error, Extended, p);
		# 	}
		# }		
    } 


