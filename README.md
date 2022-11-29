# Table of contents
1. [SSIDS](#introduction)
2. [Aim of the project](#project)
3. [Use case](#usecase)
4. [Installations](#install)
    1. [Install prerequisites for SSIDS](#prereq)
    2. [Add Fieldbus extensions](#extensions)
    3. [Configure interfaces and add project repository to path](#config)
5. [Usage](#usage)

# SSIDS <a name="introduction"></a>
SSIDS is a Python 3 project. It is a Standard Specification-based IDS, for Industrial Control Systems (ICSs). This repository contains extracts of codes and aims at presenting the concept of the project.  In this repository you will find codes for extending ZEEK IDS to industrial fieldbus detection and detection scripts for online monitoring.



# Aim of the project <a name="project"></a>





# Use case <a name="usecase"></a>

We use a Schneider Electric industrial Cartesian robot. Cartesian robots represent 40% of automatic handling solutions (Pick&Place).
The overall system is a hierarchical and distributed ICS, composed of:
* A two-axis positioning system [MAXR12](https://www.se.com/ww/en/product/MAXR12/portal-robot-lexium-max-2-directions-toothed-belt-roller-guide/) Bergerlahr.
* Two actuators (motors) and two sensors (encoders), [BSH](https://www.se.com/ww/en/download/document/0198441113837-EN/) servo motors, used for each axis to control and detect limit positions.
* Two industrial servo-variable speed controllers, [Lexium 32M](https://www.se.com/ww/en/download/document/0198441113767-EN/).
* A Programmable Logic Controller (PLC) [Modicon M340](https://www.se.com/ww/en/product-range/1468-modicon-m340/).
* A Human Machine Interface (HMI) [Magelis XBTGT 2330](https://www.se.com/ww/en/download/document/35010372K01000/).

On each loop, the actuators and and sensors are directly wired to the servo drive inputs/outputs. Communication between the PLC and the servo drives uses a CANopen fieldbus while the HMI and the PLC are connected through a Modbus TCP/IP network.

<p align="center" width="100%">
    <img width="20%" src="./Pictures/robot0.png ">
    <img width="20%" src="./Pictures/robot1.png ">
    <img width="25.5%" src="./Pictures/robot2.png ">
</p>

# Installation <a name="install"></a>


## Install prerequisites for SSIDS <a name="prereq"></a>

```
sudo apt-get install cmake make gcc g++ flex bison libpcap-dev libssl-dev python-dev swig zlib1g-dev
```
Prepare Zeek installation, but do not install it yet.

See [Zeek](https://github.com/zeek/) official github. We have Zeek version 4.1.0-dev.704, to get the exact same version:
```
git clone --recursive -b v4.1.0-dev https://github.com/zeek/zeek
```

## Configure Fieldbuses <a name="extensions"></a>

### CAN
You need a [CAN-USB](www.peak-system.com/PCAN-USB.199.0.html?&L=2) adapter to connect to the CAN fieldbus.

We need to set up the access to the CAN interface by initialising a socket and bind that socket to an interface. Install open source library [python-can](https://github.com/hardbyte/python-can).

```
pip install python-can

```
configure the IP link to specify the CAN bus bitrate, for example: 


```
sudo ip link set can0 type can bitrate 250000
sudo ip link set up can0

```

Now, we need to create a Zeek CAN Worker(software agent) in order to intercept CAN network packets; codes are in the Folder [Fieldbuses/CAN](./Fieldbuses/CAN).

To add the files to Zeek framework, copy the folder [CAN](./Fieldbuses/CAN) inside the previoulsy downloaded Zeek folder. From Zeek folder, execute:
```
mkdir src/packet_analysis/protocol/can
cp CAN/CAN.cc src/packet_analysis/protocol/can/CAN.cc
cp CAN/CAN.h src/packet_analysis/protocol/can/CAN.h
cp CAN/CMakeLists-01.txt src/packet_analysis/protocol/can/CMakeLists.txt
cp CAN/Plugin.cc src/packet_analysis/protocol/can/Plugin.cc
cp CAN/events.bif src/packet_analysis/protocol/can/events.bif
cp CAN/LinuxSLL.cc src/packet_analysis/protocol/linux_sll/LinuxSLL.cc
cp CAN/CMakeLists.txt src/packet_analysis/protocol
cp CAN/init-bare.zeek scripts/base/init-bare.zeek
cp CAN/main.zeek scripts/base/packet-protocols/linux_sll/main.zeek
```
You are ready to go!

### Modbus RTU

You need a [RS-485 to USB](https://www.gotronic.fr/art-convertisseur-usb-rs485-31533.htm) converter to connect to the Modbus RTU fieldbus.

We developped a stand-alone Modbus RTU Worker; codes are in the Folder [Fieldbuses/Modbus_RTU](./Fieldbuses/Modbus_RTU).

Configure permissions for the Modbus RTU Worker:

```
sudo chmod ugo+rw /dev/ttyUSB0

```



## Install Zeek

From Zeek folder:
```
./configure
sudo make
sudo make install
```
Add Zeek to path:
```
sudo ln --symbolic /usr/local/zeek/bin/zeek /usr/bin/zeek
sudo ln --symbolic /usr/local/zeek/bin/zeekctl /usr/bin/zeekctl
export PATH=$PATH:/usr/local/zeek/bin
```
Install broker:
```
./configure
make & sudo make install
```




# Usage <a name="usage"></a>

## server

## Set Zeek configurations

In /usr/local/zeek/etc/ you can configure the following files


##Zeekctl
```
sudo zeekctl 
deploy 
```
## Add project repository to path <a name="config"></a>



