/*******************************
tcp_client.c: the source file of the client in tcp transmission
********************************/

#include "headsock.h"

// transmission function
void str_cli(FILE *fp, int sockfd);

int main(int argc, char **argv) {
    int sockfd, ret;
    struct sockaddr_in ser_addr;
    char **pptr;
    struct hostent *sh;
    struct in_addr **addrs;

    // check input parameter
    if (argc != 2) {
        printf("parameters not match");
    }

    // get host's information from the input argument
    sh = gethostbyname(argv[1]);
    if (sh == NULL) {
        printf("error when gethostby name");
        exit(0);
    }

    // get the server(receiver)'s ip address
    addrs = (struct in_addr **)sh->h_addr_list;
    // print the remote host's information
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
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("error in socket");
        exit(1);
    }
    // ----- update ser_addr -----
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(MYTCP_PORT);
    memcpy(&(ser_addr.sin_addr.s_addr), *addrs, sizeof(struct in_addr));
    bzero(&(ser_addr.sin_zero), 8);

    // ===== step 2: create connect the socket with the server(receiver) =====
    ret =
        connect(sockfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr));
    if (ret != 0) {
        printf("connection failed\n");
        close(sockfd);
        exit(1);
    }

    // ===== step 3: send() data to server side =====
    str_cli(stdin, sockfd);

    // ===== step 4: close() socket =====
    close(sockfd);
    exit(0);
}

void str_cli(FILE *fp, int sockfd) {
    char sends[MAXSIZE];
    printf("Please input a string (less than 50 character):\n");
    if (fgets(sends, MAXSIZE, fp) == NULL) {
        printf("error input\n");
    }
    // send the string to the server(receiver)
    send(sockfd, sends, strlen(sends), 0);
    printf("send out!!\n");
}
