#ifndef _SWARM_CONF_H
#define _SWARM_CONF_H 1

// Description: Configuration file for the swarm project

// Drone IP address
#define DRONE_IP (const char*)"192.168.10.1"

// Raspberry Pi IP address on the drone network
#define PI_IP_DRONE_NET (const char*)"192.168.10.2"

// Raspberry Pi IP address on the control network
#define PI_IP_CONTROL_NET (const char*)""

// Control PC IP address
#define CONTROL_PC_IP (const char*)""

// Declare the drone's IP address
extern in_addr_t drone_addr;
extern in_addr_t pi_addr_drone_net;
extern in_addr_t pi_addr_control_net;
extern in_addr_t control_pc_addr;

void init_addresses();

#endif