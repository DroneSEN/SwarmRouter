# SwarmRouter

A program to route packets from the Drone network to a Control network using a Raspberry Pi as a gateway.


## How to use

1. Connect the Raspberry Pi to the Drone wifi.
2. Connect the Raspberry Pi to the Control network with Ethernet
3. Configure the program and compile it : `make swarm_router`
    (A configuration file will be added in next release)
4. Run the program

## Configuration

The configuration is currently made in the swarm_conf.h file.
Addresses and ports can be configured.