//******************************************************************
// Copyright (C) 2011-2020 Morten Jagd Christensen
//******************************************************************

#include <sys/types.h>          // for type definitions
#include <sys/socket.h>         // for socket API calls
#include <netinet/in.h>         // for address structs
#include <arpa/inet.h>          // for sockaddr_in
#include <stdio.h>              // for printf() and fprintf()
#include <stdlib.h>             // for atoi()
#include <string.h>             // for strlen()
#include <unistd.h>             // for close()
#include <sys/timeb.h>
#include <stdlib.h>             // for system()

const int MaxLen{1400};         // maximum receive string size

int rxpackets;
int rxbytes;

int main(int argc, char *argv[])
{
    int             sock;                 // socket descriptor
    int             flag_on = 1;          // socket option flag
    struct sockaddr_in mc_addr;           // socket address structure
    char            recv_str[MaxLen + 1]; // buffer to receive string
    int             recv_len;             // length of string received
    struct ip_mreq  mc_req;               // multicast request structure
    char           *mc_addr_str;          // multicast IP address
    unsigned short  mc_port;              // multicast port
    struct sockaddr_in from_addr;         // packet source
    unsigned int    from_len;             // source addr length

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Multicast IP> <Multicast Port>\n", argv[0]);
        exit(1);
    }

    mc_addr_str = argv[1];      // arg 1: multicast ip address
    mc_port = atoi(argv[2]);    // arg 2: multicast port number


    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket() failed");
        exit(1);
    }

    if ((setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag_on, sizeof(flag_on))) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(1);
    }

    if ((setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &flag_on, sizeof(flag_on))) < 0) {
        perror("setsockopt(SO_REUSEPORT) failed");
        exit(1);
    }

    // construct a multicast address structure
    memset(&mc_addr, 0, sizeof(mc_addr));
    mc_addr.sin_family = AF_INET;
    mc_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    mc_addr.sin_port = htons(mc_port);

    // bind to multicast address to socket
    if ((bind(sock, (struct sockaddr *)&mc_addr, sizeof(mc_addr))) < 0) {
        perror("bind() failed");
        exit(1);
    }

    // construct an IGMP join request structure
    mc_req.imr_multiaddr.s_addr = inet_addr(mc_addr_str);
    mc_req.imr_interface.s_addr = htonl(INADDR_ANY);

    // send an ADD MEMBERSHIP message via setsockopt
    if ((setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&mc_req, sizeof(mc_req))) < 0) {
        perror("setsockopt(IP_ADD_MEMBERSHIP) failed");
        exit(1);
    }

    for (;;) {
        memset(recv_str, 0, sizeof(recv_str));
        from_len = sizeof(from_addr);
        memset(&from_addr, 0, from_len);

        if ((recv_len = recvfrom(sock, recv_str, MaxLen, 0, (struct sockaddr *)&from_addr, &from_len)) < 0) {
            perror("recvfrom() failed");
            exit(1);
        }

        rxbytes += strlen(recv_str);
        rxpackets++;

        printf("Packet %d (%d bytes): %s\n", rxpackets, recv_len, recv_str);
        system(recv_str);
    }
}
