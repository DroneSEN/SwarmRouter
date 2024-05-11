#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "udp_proxy.h"

/**
 * Create the UDP proxy between device 1 and device 2
 * 
 * [IN]  Address of device 1
 * [IN]  Address of device 2
 * [IN]  Host address on the same network as Device 1  
 * [IN]  Host address on the same network as Device 2
 * [IN]  Port on which the proxy will route packets
 * [OUT] Pointer to the UDP state structure
 */
int create_udp_proxy(in_addr_t device1_addr, 
                        in_addr_t device2_addr,
                        in_addr_t host_addr_net_1,
                        in_addr_t host_adrr_net_2,
                        uint16_t port,
                        char* device1_name,
                        char* device2_name,
                        UDP_PROXY_STATE* udp_proxy)
{
    // Save parameters in the UDP state structure
    udp_proxy->device1_addr = device1_addr;
    udp_proxy->device2_addr = device2_addr;
    udp_proxy->host_addr_net1 = host_addr_net_1;
    udp_proxy->host_addr_net2 = host_adrr_net_2;
    udp_proxy->port = port;

    // Save the names
    strcpy(udp_proxy->device1_name, device1_name);
    strcpy(udp_proxy->device2_name, device2_name);

    // Create the Host <-> Device 1 socket
    udp_proxy->host_device1_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udp_proxy->host_device1_socket == -1) {
        printf("Error creating Host <---> %s socket\n", udp_proxy->device1_name);
        return -1;
    }
    printf("Host <---> %s socket created\n", udp_proxy->device1_name);

    // Binding Host <-> Device 1

    // Define the 4 different socket address

    // Device 1 UDP socket
    udp_proxy->device1_sock_addr.sin_family = AF_INET;
    udp_proxy->device1_sock_addr.sin_port = htons(port);
    udp_proxy->device1_sock_addr.sin_addr.s_addr = device1_addr;

    // Host UDP socket on network 1
    udp_proxy->host_net1_sock_addr.sin_family = AF_INET;
    udp_proxy->host_net1_sock_addr.sin_port = htons(port);
    udp_proxy->host_net1_sock_addr.sin_addr.s_addr = host_addr_net_1;

    // Host UDP socket on network 2
    udp_proxy->device2_sock_addr.sin_family = AF_INET;
    udp_proxy->device2_sock_addr.sin_port = htons(port);
    udp_proxy->device2_sock_addr.sin_addr.s_addr = device2_addr;

    // Device 2 UDP socket
    udp_proxy->host_net2_sock_addr.sin_family = AF_INET;
    udp_proxy->host_net2_sock_addr.sin_port = htons(port);
    udp_proxy->host_net2_sock_addr.sin_addr.s_addr = host_adrr_net_2;

    // Bind the Device 1 and Host sockets
    if(bind(udp_proxy->host_device1_socket, (struct sockaddr*)&(udp_proxy->host_net1_sock_addr), sizeof(udp_proxy->host_net1_sock_addr)) < 0) {
        printf("Error binding Host <---> %s socket to port %d\n", udp_proxy->device1_name, port);
        return -1;
    }

    printf("Successfuly binded Host <---> %s socket to port %d\n", udp_proxy->device1_name, port);


    // Create the Host <-> Device 2 socket
    udp_proxy->host_device2_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udp_proxy->host_device2_socket == -1) {
        printf("Error creating Host <--> %s socket\n", udp_proxy->device2_name);
        return -1;
    }
    printf("Host <---> %s socket created\n", udp_proxy->device2_name);

    // Bind the Device 2 and Host sockets
    if(bind(udp_proxy->host_device2_socket, (struct sockaddr*)&(udp_proxy->host_net2_sock_addr), sizeof(udp_proxy->host_net2_sock_addr)) < 0) {
        printf("Error binding Host <---> %s socket to port %d\n", udp_proxy->device2_name, port);
        return -1;
    }

    printf("Successfuly binded Host <---> %s socket to port %d\n", udp_proxy->device2_name, port);

    printf("\nUDP Proxy between %s and %s on port %d has been successfuly configured !\n", device1_name, device2_name, port);

    return 0;
}

struct sockaddr sender_sock_addr;
socklen_t sender_sock_addr_len;

void run_udp_proxy(UDP_PROXY_STATE* udp_proxy)
{
    while(1)
    {

        // Step 1
        // Device 1 --> Device 2

        // Receive a command from the first device
        char data_buffer[MAX_UDP_PROXY_BUFFER_SIZE];
        ssize_t data_buffer_size;

        // The receive shall be non blocking
        data_buffer_size = recvfrom(udp_proxy->host_device1_socket, 
                            data_buffer, sizeof(data_buffer), 
                            MSG_DONTWAIT, 
                            &sender_sock_addr,
                            &sender_sock_addr_len);

        if(data_buffer_size > 0) {
            printf("Received %d bytes from %s\n", (int)data_buffer_size, udp_proxy->device1_name);

            // Send the command to the 2nd device
            ssize_t size_sent;
            size_sent = sendto(udp_proxy->host_device2_socket, data_buffer, data_buffer_size, 0, (struct sockaddr*)&(udp_proxy->device2_sock_addr), sizeof(udp_proxy->device2_sock_addr));
        }

        // Step 2
        // Device 1 <-- Device 2

        // Receive a response from the drone
        // The receive shall be non blocking
        data_buffer_size = recvfrom(udp_proxy->host_device2_socket, 
                                    data_buffer, 
                                    sizeof(data_buffer),
                                    MSG_DONTWAIT, 
                                    &sender_sock_addr, 
                                    &sender_sock_addr_len);

        if(data_buffer_size > 0) {
            printf("Received %d bytes from %s\n", (int)data_buffer_size, udp_proxy->device2_name);

            // Send the response to the Control PC
            ssize_t size_sent_pc;
            size_sent_pc = sendto(udp_proxy->host_device2_socket, data_buffer, data_buffer_size, 0, (struct sockaddr*)&(udp_proxy->device2_sock_addr), sizeof(udp_proxy->device2_sock_addr));
        }
    }
}