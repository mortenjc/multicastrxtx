# multicastrxtx
Examples of multicast transmitter and reciever

## build
    > mkdir build
    > cd build
    > cmake ..
    > make

## run receiver(s)
The receiver(s) listen for a line on the specified multicast
address and udp port. Then prints the line.

    > ./mcrecv multicast_ip udp_port
    > ./mcrecv 224.2.3.4 9000

## run transmitter
The transmitter reads a line from the console and sends this on the
specified multicast address and udp port

    > ./mcsend multicast_ip udp_port
    > ./mcsend 224.2.3.4 9000

The socket options SO_REUSEADDR and SO_REUSEPORT allows multiple processes
on the same host to listen on the same port and multicast address simultaneously.
