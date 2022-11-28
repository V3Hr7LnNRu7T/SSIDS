
#illegal speed in one direction
cansend can1 201#1f420600bf27fdff
sleep 0.4
#stop command
cansend can1 201#9f420000bf27fdff

sleep 2
#normal speed in the other direction
cansend can1 201#1f420100bf27fdff
sleep 0.4
#stop command
cansend can1 201#9f420000bf27fdff
