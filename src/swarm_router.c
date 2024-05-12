#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/signal.h>

// Configuration file
#include "swarm_conf.h"

#include "drone_cmd_router.h"
#include "udp_proxy.h"

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

UDP_PROXY_STATE drone_cmd_proxy;
UDP_PROXY_STATE drone_state_proxy;
UDP_PROXY_STATE drone_video_proxy;

// Main function
int main() {

    // Child process PIDs
    pid_t route_commands_pid;
    pid_t route_state_pid;
    pid_t route_video_pid;

    // Initialize addresses
    init_addresses();

    // TEST ZONE
    int state;

    // Drone Commands proxy (Port 8889)
    state = create_udp_proxy(drone_addr,
                            control_pc_addr, 
                            pi_addr_drone_net, 
                            pi_addr_control_net, 
                            DRONE_CMD_PORT, 
                            "Drone",
                            "Control PC",
                            &drone_cmd_proxy);

    if(state == -1)
        return EXIT_FAILURE;

    // Drone State proxy
    state = create_udp_proxy(drone_addr,
                            control_pc_addr, 
                            pi_addr_drone_net, 
                            pi_addr_control_net, 
                            DRONE_STATE_PORT, 
                            "Drone",
                            "Control PC",
                            &drone_state_proxy);

    if(state == -1)
        return EXIT_FAILURE;

    // Drone video proxy
    state = create_udp_proxy(drone_addr,
                            control_pc_addr, 
                            pi_addr_drone_net, 
                            pi_addr_control_net, 
                            DRONE_VIDEO_PORT, 
                            "Drone",
                            "Control PC",
                            &drone_video_proxy);

    if(state == -1)
        return EXIT_FAILURE;

    // Run proxies
    
    // Command proxy
    route_commands_pid = fork();
    if(route_commands_pid == 0)
    {
        run_udp_proxy(&drone_cmd_proxy);
        return EXIT_SUCCESS;
    }

    // State proxy
    route_state_pid = fork();
    if(route_state_pid == 0)
    {
        run_udp_proxy(&drone_state_proxy);
        return EXIT_SUCCESS;
    }

    // Video proxy
    route_video_pid = fork();
    if(route_video_pid == 0)
    {
        run_udp_proxy(&drone_video_proxy);
        return EXIT_SUCCESS;
    }    

    // Main program
    // Continue parent process
    
    // Wait for user to press "s" to stop the program
    printf("Press 's' to stop the program\n");

    char c;
    while(1)
    {
        c = getchar();
        if(c == 's')
        {
            printf("Stopping the program\n");

            // Kill the subprocess
            kill(route_commands_pid, SIGKILL);
            kill(route_state_pid, SIGKILL);
            kill(route_video_pid, SIGKILL);
            
            break;
        }
    }

    // Return success
    return EXIT_SUCCESS;
}