# Profile pos : absolute
# authorized positions
# 0<x<260   et    0<y<193 mm


# drive 1 (axis y) (target = 44mm)
cansend can1 201#0142f401630c0100
sleep 5
#target = -50mm  
cansend can1 201#8142f4019ccefeff
sleep 5


