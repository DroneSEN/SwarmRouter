#ifndef _UDP_PROXY_H
#define _UDP_PROXY_H 1

#include <netinet/in.h>

#define MAX_UDP_PROXY_BUFFER_SIZE 3000
#define MAX_UDP_PROXY_DEVICE_NAME_LEN 20

typedef struct {
    // Address of the device 1 (packets received from addr1 are sent to addr2 and vice versa)
    in_addr_t device1_addr;

    // Address of the device 2 (packets received from addr2 are sent to addr1 and vice versa)
    in_addr_t device2_addr;

    // Address of the Raspberry PI on the network of the device 1
    in_addr_t host_addr_net1;

    // Address of the Raspberry PI on the network of the device 2 (can be the same as host_addr_net_1)
    in_addr_t host_addr_net2;

    // UDP socket between Host and Device 1
    int host_device1_socket;

    // UDP socket between Host and Device 2
    int host_device2_socket;

    // Port on which the Host and Device 1 communicate
    uint16_t device1_port;

    // Port on which the Host and Device 2 communicate
    uint16_t device2_port;

    // Computed socket addresses
    struct sockaddr_in device1_sock_addr;
    struct sockaddr_in host_net1_sock_addr;
    struct sockaddr_in host_net2_sock_addr;
    struct sockaddr_in device2_sock_addr;

    // Device 1 name
    char device1_name[MAX_UDP_PROXY_DEVICE_NAME_LEN];

    // Device 2 name
    char device2_name[MAX_UDP_PROXY_DEVICE_NAME_LEN];

} UDP_PROXY_STATE;

/**
 * Create the UDP proxy between device 1 and device 2
 * 
 * [IN]  Address of device 1
 * [IN]  Address of device 2
 * [IN]  Host address on the same network as Device 1  
 * [IN]  Host address on the same network as Device 2
 * [IN]  Port on which the Host and Device 1 communicate
 * [IN]  Port on which the Host and Device 2 communicate
 * [IN]  Name of device 1
 * [IN]  Name of device 2
 * [OUT] Pointer to the UDP state structure
 */
int create_udp_proxy(const in_addr_t device1_addr, 
                        const in_addr_t device2_addr,
                        const in_addr_t host_addr_net_1,
                        const in_addr_t host_adrr_net_2,
                        const uint16_t device1_port,
                        const uint16_t device2_port,
                        const char* device1_name,
                        const char* device2_name,
                        UDP_PROXY_STATE* udp_proxy);

void run_udp_proxy(UDP_PROXY_STATE* udp_proxy);

#endif