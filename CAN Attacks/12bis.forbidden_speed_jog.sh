
#forbidden speed
cansend can1 201#1f420600bf27fdff

#ack fault
cansend can1 201#9f4a0000bf27fdff
cansend can1 201#9f420000bf27fdff

#normal speed
cansend can1 201#1f420100bf27fdff
sleep 0.3
cansend can1 201#9f420000bf27fdff

#doc - data[2]
Value 0: No movement
Value 1: Slow movement
in positive direction
Value 2: Slow movement
in negative direction
Value 5: Fast movement
in positive direction
Value 6: Fast movement
in negative direction
