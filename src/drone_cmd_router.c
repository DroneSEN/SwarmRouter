#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "drone_cmd_router.h"

int route_commands()
{
    /*
     * The following code is used to send a command to the drone
     * (Temporary code to test the connection between the Raspberry Pi and the drone)
     * 
     *        Control PC   <---> Raspberry Pi <---> Drone   [Port:8889]
     */

    // Create a UDP socket to send and receive commands to/from the drone
    int pi_drone_command_socket;
    pi_drone_command_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (pi_drone_command_socket == -1) {
        printf("Error creating PI-Drone command socket\n");
        return EXIT_FAILURE;
    }
    printf("PI-Drone Command socket created\n");

    // Create a UDP socket to send and receive commands to/from the Control PC
    int pi_pc_command_socket;
    pi_pc_command_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (pi_pc_command_socket == -1) {
        printf("Error creating PI-PC command socket\n");
        return EXIT_FAILURE;
    }
    printf("PI-PC Command socket created\n");
    
    // Specify an address for the local PI-Drone socket
    struct sockaddr_in local_pi_drone_command_sock_addr;
    local_pi_drone_command_sock_addr.sin_family = AF_INET;
    local_pi_drone_command_sock_addr.sin_port = htons(8889);
    local_pi_drone_command_sock_addr.sin_addr.s_addr = pi_addr_drone_net;

    // Specify an address for the Drone socket used for control
    struct sockaddr_in remote_pi_drone_command_sock_addr;
    remote_pi_drone_command_sock_addr.sin_family = AF_INET;
    remote_pi_drone_command_sock_addr.sin_port = htons(8889);
    remote_pi_drone_command_sock_addr.sin_addr.s_addr = drone_addr;

    // Specify an address for the local PI-PC socket
    struct sockaddr_in local_pi_pc_cmd_sock_addr;
    local_pi_pc_cmd_sock_addr.sin_family = AF_INET;
    local_pi_pc_cmd_sock_addr.sin_port = htons(8889);
    local_pi_pc_cmd_sock_addr.sin_addr.s_addr = pi_addr_control_net;

    // Specify an address for the Control PC socket
    struct sockaddr_in remote_pi_pc_cmd_sock_addr;
    remote_pi_pc_cmd_sock_addr.sin_family = AF_INET;
    remote_pi_pc_cmd_sock_addr.sin_port = htons(8889);
    remote_pi_pc_cmd_sock_addr.sin_addr.s_addr = control_pc_addr;

    // Bind the socket Raspberry Pi <---> Drone
    if(bind(pi_drone_command_socket, (struct sockaddr*)&local_pi_drone_command_sock_addr, sizeof(local_pi_drone_command_sock_addr)) < 0) {
        printf("Error binding PI-Drone Command socket\n");
        return EXIT_FAILURE;
    }

    printf("Bind to port 8889 done\n");

    // Bind the socket Control PC <---> Rasberry Pi
    if(bind(pi_pc_command_socket, (struct sockaddr*)&local_pi_pc_cmd_sock_addr, sizeof(local_pi_pc_cmd_sock_addr)) < 0) {
        printf("Error binding PI-PC Command socket\n");
        return EXIT_FAILURE;
    }

    printf("Bind to port 8889 done\n");

    // Loop in the following order:
    // 1. Receive a command from the Control PC
    // 2. Send the command to the drone
    // 3. Receive the response from the drone
    // 4. Send the response to the Control PC

    while(1)
    {

        // Receive a command from the Control PC
        char pc_command[50];
        ssize_t size_recv;
        // The receive shall be non blocking
        size_recv = recvfrom(pi_pc_command_socket, 
                            pc_command, sizeof(pc_command), 
                            MSG_DONTWAIT, 
                            (struct sockaddr*)&remote_pi_pc_cmd_sock_addr, sizeof(remote_pi_pc_cmd_sock_addr));

        if(size_recv > 0) {
            printf("Received %d bytes from the Control PC\n", (int)size_recv);

            // Send the command to the drone
            ssize_t size_sent;
            size_sent = sendto(pi_drone_command_socket, pc_command, strlen(pc_command), 0, (struct sockaddr*)&remote_pi_drone_command_sock_addr, sizeof(remote_pi_drone_command_sock_addr));
        } else {
            printf("No command received from the Control PC\n");
        }

        // Receive a response from the drone
        char drone_response[50];
        ssize_t size_recv_drone;
        // The receive shall be non blocking
        size_recv_drone = recvfrom(pi_drone_command_socket, drone_response, sizeof(drone_response), MSG_DONTWAIT, (struct sockaddr*)&remote_pi_drone_command_sock_addr, sizeof(remote_pi_drone_command_sock_addr));

        if(size_recv_drone > 0) {
            printf("Received %d bytes from the drone\n", (int)size_recv_drone);

            // Send the response to the Control PC
            ssize_t size_sent_pc;
            size_sent_pc = sendto(pi_pc_command_socket, drone_response, strlen(drone_response), 0, (struct sockaddr*)&remote_pi_pc_cmd_sock_addr, sizeof(remote_pi_pc_cmd_sock_addr));
        } else {
            printf("No response received from the drone\n");
        }
    }

    return EXIT_SUCCESS;
}