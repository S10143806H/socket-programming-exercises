# socket-programming-exercises
Develop a socket program in UNIX/Linux that uses TCP and UDP as the transport protocol for transferring message between a client and server. 
There are 3 exercise for help with understand TCP & UDP socket programming listed in 3 seperated folders.

# Ex1
## Introduction
This example is to show how to transmit a short packet using TCP and UDP Transport Protocols.
Input a string (< 50 characters) at the client end, you will receive the string at the server.
## Build Instruction
![socket_instruction](https://user-images.githubusercontent.com/9852678/163305334-d5217a09-3292-433f-b111-6ace9272f824.png)

# Ex2
## Introduction
This example is to show how to transmit a large packet using **UDP and TCP**. 

The large packet is achieved from a file which is nearly 30000 bytes (if larger, the MAXLEN in headsock.h should be also modified). 
The file name is "myfile.txt", the client end try to send the file to the server in one packet.

At the receiver, the function "recv" is called several times until all the data is received (the packet is larger than the TCP receiver buffer). 
The received data is stored in file "myTCPreceive.txt".

# Ex3
## Introduction
This example is to show how to transmit a large file using **small packets**. 

The file to be sent is "myfile.txt", the received data is stored in "myTCPreceive.txt" in TCP case and in "myUDPreceive.txt" in UDP case.
The packet size is fixed at 100 bytes per packets. The receiver transmit the acknolegement to sender when the last byte is received. 
In test, the file size is 50554 bytes. In TCP case, all data is received without error. 
