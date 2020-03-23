/********************************************************************
 Copyright (C) 2011 Morten Jagd Christensen
 all rights reserved
********************************************************************/

#include <sys/types.h>          /* for type definitions          */
#include <sys/socket.h>         /* for socket API function calls */
#include <netinet/in.h>         /* for address structs           */
#include <arpa/inet.h>          /* for sockaddr_in               */
#include <stdio.h>              /* for printf()                  */
#include <stdlib.h>             /* for atoi()                    */
#include <string.h>             /* for strlen()                  */
#include <unistd.h>             /* for close()                   */

#define MAX_LEN  1400           /* maximum string size to send   */

#ifdef LWEFORMAT
    #define LWEOFS 6
#else 
    #define LWEOFS 0
#endif


void hexdump(char * a, int len)
{
    for (int i=0; i<len; i++)
    {
        if ((i % 16) ==0)
            printf("\n%04d: ", i);
        printf("%02x ", a[i]);        
    }
}

int main(int argc, char *argv[])
{
    int             sock;               /* socket descriptor            */
    char            send_str[MAX_LEN];  /* string to send               */
    struct sockaddr_in mc_addr;         /* socket address structure     */
    unsigned int    send_len;           /* length of string to send     */
    char           *mc_addr_str;        /* multicast IP address         */
    unsigned short  mc_port;            /* multicast port               */
    unsigned char   mc_ttl = 1;         /* time to live (hop count)     */


    if (argc != 3) 
    {
        fprintf(stderr, "Usage: %s <Multicast IP> <Multicast Port>\n", argv[0]);
        exit(1);
    }

    mc_addr_str = argv[1];              /* arg 1: multicast IP address  */
    mc_port = atoi(argv[2]);            /* arg 2: multicast port number */

    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        perror("socket() failed");
        exit(1);
    }

    if ((setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&mc_ttl, sizeof(mc_ttl))) < 0)
    {
        perror("setsockopt() MULTICAST TTL failed");
        exit(1);
    }

    memset(&mc_addr, 0, sizeof(mc_addr));
    mc_addr.sin_family      = AF_INET;
    mc_addr.sin_addr.s_addr = inet_addr(mc_addr_str);
    mc_addr.sin_port        = htons(mc_port);

    memset(send_str, 0, sizeof(send_str));

    while (fgets(send_str+LWEOFS, MAX_LEN, stdin))
    {
        send_len = strlen(send_str+LWEOFS)+LWEOFS;

#ifdef LWEFORMAT
        strcpy(send_str, "UdPbC");
#endif

        /* send string to multicast address */
        if ((sendto(sock, send_str, send_len, 0, (struct sockaddr *)&mc_addr, sizeof(mc_addr))) != send_len)
        {
            perror("sendto() sent incorrect number of bytes");
            exit(1);
        }

        /* clear send buffer */
        memset(send_str, 0, sizeof(send_str));
    }

    close(sock);
    exit(0);
}
