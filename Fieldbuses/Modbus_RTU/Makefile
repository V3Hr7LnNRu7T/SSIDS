all: modbus_rtu_probe modbus_rtu_pub


modbus_rtu_probe : modbus_rtu_probe.c
	gcc -g modbus_rtu_probe.c -lrt -o modbus_rtu_probe

modbus_rtu_pub : modbus_rtu_pub.cc
	g++ -c -g -std=c++17  -o modbus_rtu_pub.cc.o modbus_rtu_pub.cc
	/usr/bin/c++   -Wall -Wno-unused -Wno-register -Werror=vla -std=c++17 -O2 -g -DNDEBUG   modbus_rtu_pub.cc.o  -o modbus_rtu_pub  /usr/local/lib/libbroker.so.2.1 /usr/lib/x86_64-linux-gnu/libssl.so /usr/lib/x86_64-linux-gnu/libcrypto.so /usr/local/lib/libcaf_openssl.so.0.18.3 /usr/local/lib/libcaf_io.so.0.18.3 /usr/local/lib/libcaf_core.so.0.18.3 -lpthread /usr/lib/x86_64-linux-gnu/libssl.so /usr/lib/x86_64-linux-gnu/libcrypto.so
	
	
	
	
