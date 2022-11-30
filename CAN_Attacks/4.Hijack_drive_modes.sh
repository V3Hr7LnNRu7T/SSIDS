#start jog - var 1 (axe y)
cansend can1 201#9f420100bf27fdff


#pp (target = 44mm)
cansend can1 201#0142f401630c0100
cansend can1 201#8142f401630c0100

sleep 1

#stop jog
cansend can1 201#9f420000bf27fdff

#or move jog manually
