#include <netinet/in.h>
#include <arpa/inet.h>


#include "swarm_conf.h"

in_addr_t drone_addr;
in_addr_t pi_addr_drone_net;
in_addr_t pi_addr_control_net;
in_addr_t control_pc_addr;

void init_addresses()
{
    // Define the drone's IP address
    drone_addr = inet_addr(DRONE_IP);
    pi_addr_drone_net = inet_addr(PI_IP_DRONE_NET);
    pi_addr_control_net = inet_addr(PI_IP_CONTROL_NET);
    control_pc_addr = inet_addr(CONTROL_PC_IP);
}