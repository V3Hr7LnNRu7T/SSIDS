# Table of contents
1. [SSIDS](#introduction)
2. [Aim of the project](#project)
3. [Use case](#usecase)
4. [Installations](#install)
    1. [Install prerequisites for SSIDS](#prereq)
    2. [Configure Fieldbuses](#fieldbuses)
    3. [Install Zeek](#zeek)
5. [Usage](#usage)
    1. [Server](#server)
    2. [Publishers](#pub)
    3. [Subscribers](#sub)

# SSIDS <a name="introduction"></a>
SSIDS is a Zeek/Python 3 project. It is a Standard Specification-based IDS, for Industrial Control Systems (ICSs). This repository contains extracts of codes and aims at presenting the concept of the project.  In this repository you will find codes for extending ZEEK IDS to industrial fieldbus detection and detection scripts for online monitoring.



# Aim of the project <a name="project"></a>

The aim of the project is to present a detection framework for ICS. The steps can be applied to any industrial system.
In order to illustrate better the project, the steps are detailed through a use case.

With the use case, we capture network traffic at Fieldbus and Ethernet TCP/IP levels, we use Zeek to capture data and dispatch it to our detection scripts. Only extracts of the detection scripts are given here.
The system is monitored at runtime.

Attacks scripts are provided in the folder [CAN_Attacks](./CAN_Attacks).

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

## Configure Fieldbuses <a name="fieldbuses"></a>

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

Install and configure permissions for the Modbus RTU Worker:

```
make
sudo chmod ugo+rw /dev/ttyUSB0

```


## Install Zeek <a name="zeek"></a>

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

We use multiple threads simultaneously:
* A server that collects all the messages/topics
* Worker(s) that capture network traffic and publish messages to the server
* Detection scripts that subscribe to topics in order to monitor the system during its execution.

## Server  <a name="server"></a>

execute [server.py](./src/server.py)

## Publishers  <a name="pub"></a>

For traffic capture on one interface only, a simple command line can be used. For example if we want to monitor can0 interface with a [specific script](./src/Workers/robot1_publishers.zeek):
```
sudo zeek -i can0 robot1_publishers.zeek
```

Otherwise, for mulit-interfaces capture, Zeekctl has to be used. In /usr/local/zeek/etc/ you need to fill the configuration files. Personal scripts can be added in /usr/local/zeek/share/zeek/policy/, they will be launched automatically.
Then:
```
sudo zeekctl 
deploy 
```

## Subscribers  <a name="sub"></a>

Add current repository to PYTHONPATH in order to use absolute imports (source export PYTHONPATH=yourpath...)

Execute subscriber(s).



