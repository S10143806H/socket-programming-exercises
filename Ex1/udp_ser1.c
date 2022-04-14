/**************************************
udp_ser.c: the source file of the server in udp transmission
**************************************/
#include "headsock.h"

void str_ser1(int sockfd);  // transmitting and receiving function

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in my_addr;

    // ===== step 1: create socket =====
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("error in socket");
        exit(1);
    }
    // ----- update my_addr -----
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYUDP_PORT);
    // INADDR_ANY: in case the system has multiple IPs
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 8);

    // ===== step 2: bind socket =====
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) ==
        -1) {
        printf("error in binding");
        exit(1);
    }

    // ===== step 3: keep listen & recvfrom() client =====
    printf("start receiving\n");
    while (1) {
        str_ser1(sockfd);
    }

    // ===== step 4: close() socket =====
    close(sockfd);
    exit(0);
}

void str_ser1(int sockfd) {
    char recvs[MAXSIZE];
    int n = 0, len;
    struct sockaddr_in addr;

    len = sizeof(struct sockaddr_in);

    if ((n = recvfrom(sockfd, &recvs, MAXSIZE, 0, (struct sockaddr *)&addr,
                      &len)) == -1) {  // receive the packet
        printf("error receiving");
        exit(1);
    }
    recvs[n] = '\0';
    printf("the received string is :\n%s", recvs);
}
