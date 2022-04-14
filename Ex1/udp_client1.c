/*******************************
udp_client.c: the source file of the client in udp
********************************/

#include "headsock.h"

void str_cli1(FILE *fp, int sockfd, struct sockaddr *addr, int addrlen,
              int *len);

int main(int argc, char *argv[]) {
    int len, sockfd;
    struct sockaddr_in ser_addr;
    char **pptr;
    struct hostent *sh;
    struct in_addr **addrs;

    // check input parameter
    if (argc != 2) {
        printf("parameters not match.");
        exit(0);
    }

    // get host's information and store into sh
    if ((sh = gethostbyname(argv[1])) == NULL) {
        printf("error when gethostbyname");
        exit(0);
    }

    // get the client (sender)'s ip address
    addrs = (struct in_addr **)sh->h_addr_list;
    printf("canonical name: %s\n", sh->h_name);
    for (pptr = sh->h_aliases; *pptr != NULL; pptr++)
        // printf socket information
        printf("the aliases name is: %s\n", *pptr);
    switch (sh->h_addrtype) {  // decide host address type
        case AF_INET:
            printf("AF_INET\n");  // internet family
            break;
        default:
            printf("unknown addrtype\n");
            break;
    }

    // ===== step 1: create socket =====
    // socket(family, type, protocol=0)
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("error in socket");
        exit(1);
    }
    // ----- update ser_addr -----
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(MYUDP_PORT);
    // allocate memeory location for Internet address
    memcpy(&(ser_addr.sin_addr.s_addr), *addrs, sizeof(struct in_addr));
    bzero(&(ser_addr.sin_zero), 8);

    // ===== step 2: receive user input and sendto() server side =====
    str_cli1(stdin, sockfd, (struct sockaddr *)&ser_addr,
             sizeof(struct sockaddr_in), &len);

    // ===== step 3: close() socket =====
    close(sockfd);
    exit(0);
}

void str_cli1(FILE *fp, int sockfd, struct sockaddr *addr, int addrlen,
              int *len) {
    char sends[MAXSIZE];
    printf("Please input a string (less than 50 characters):\n");
    if (fgets(sends, MAXSIZE, fp) == NULL) {  // detect characters in sends
        printf("error input\n");
    }
    // addr: server address (remote socket)
    sendto(sockfd, &sends, strlen(sends), 0, addr, addrlen);
    printf("send out!!\n");
}
