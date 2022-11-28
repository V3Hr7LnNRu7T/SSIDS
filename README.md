# Table of contents
1. [SSIDS](#introduction)
2. [Aim of the project](#project)
3. [Use case](#usecase)
4. [Usage](#usage)
    1. [Install prerequisites for SSIDS](#prereq)
    2. [Add Fieldbus extensions](#extensions)
    3. [Configure interfaces and add project repository to path](#config)


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



# Usage <a name="usage"></a>

## Install prerequisites for SSIDS <a name="prereq"></a>

```
sudo apt install python-dev
sudo apt install python-pip
```

* Install Zeek

See [Zeek](https://github.com/zeek/) official github.



## Add Fieldbus extensions <a name="extensions"></a>

## Configure interface and add project repository to path <a name="config"></a>



