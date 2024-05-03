#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Configuration file
#include "swarm_conf.h"

#include "drone_cmd_router.h"


/*
 * This program is used to forward the Tello state to the Control PC
 * The program will receive the Tello state from the drone and forward it to the Control PC
 * The program will also send a command to the drone to start sending the state
 * The program will run on the Raspberry Pi
 * 
 * The program relies on 2 networks :
 * - The network between the Raspberry Pi and the drone (Drone Network)
 * - The network between the Raspberry Pi and the Control PC (Control Network)
*/

// Main function
int main() {

    // Initialize addresses
    init_addresses();

    // Start the subprocess for routing commands
    route_commands();

    return EXIT_SUCCESS;

    // Tello State

    // Create a socket to receive incoming stream on port 8890 (Tello state)
    // Create UDP socket
    int rx_state_socket;
    rx_state_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (rx_state_socket == -1) {
        printf("Error creating state socket\n");
        return -1;
    }

    printf("Socket created\n");

    // Specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8890);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket to our specified IP and port
    if(bind(rx_state_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Error binding state socket\n");
        return -1;
    }
    
    printf("Bind to port 8890 done\n");

    char buffer[3000];

    int max_packets = 50;
    int curr_packets = 0;

    while(curr_packets < max_packets)
    {
        ssize_t nbytes = recvfrom(rx_state_socket, buffer, 3000, 0, NULL, NULL);

        if(nbytes > 0)
        {
            fprintf(stdout, "Received %d bytes\n", (int)nbytes);

            // Foward the packet to UDP server on Control PC
            
        }

        curr_packets++;

    }

    // // Forward the packet to the Control PC
    // // Create a socket to send the state to the Control PC
    // int tx_state_socket;
    // tx_state_socket = socket(AF_INET, SOCK_DGRAM, 0);
    // if (tx_state_socket == -1) {
    //     printf("Error creating control socket\n");
    //     return -1;
    // }

    // printf("Control socket created\n");

    // // Specify an address for the socket
    // struct sockaddr_in control_address;
    // control_address.sin_family = AF_INET;
    // control_address.sin_port = htons(8891);
    // control_address.sin_addr.s_addr = drone_addr;

    // // Send the state to the Control PC
    // sendto(tx_state_socket, buffer, nbytes, 0, (struct sockaddr*)&control_address, sizeof(control_address));

    // printf("Forward %ld bytes to Control PC\n", nbytes);

    // Close both sockets
    close(rx_state_socket);
    // close(tx_state_socket);

    // Return success
    return EXIT_SUCCESS;
}