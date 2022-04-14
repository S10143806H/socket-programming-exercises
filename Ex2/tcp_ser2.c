/**********************************
tcp_ser.c: the source file of the server in tcp transmission for a large packet
***********************************/

#include "headsock.h"
#define BACKLOG 10

void str_ser(int sockfd);  // transmitting and receiving function

int main(void) {
    int sockfd, con_fd, ret;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    int sin_size;
    pid_t pid;

    // ===== step 1: create socket =====
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("error in socket!");
        exit(1);
    }
    // ----- update my_addr -----
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYTCP_PORT);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&(my_addr.sin_zero), 8);

    // ===== step 2: bind socket =====
    ret = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));
    if (ret < 0) {
        printf("error in binding");
        exit(1);
    }

    // ===== step 3: listen for client socket =====
    // inform the OS that socket is ready for request
    ret = listen(sockfd, BACKLOG);
    if (ret < 0) {
        printf("error in listening");
        exit(1);
    }
    printf("receiving start\n");
    while (1) {
        sin_size = sizeof(struct sockaddr_in);

        // ===== step 4: accept the packet =====
        // connect to an incoming request
        con_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (con_fd < 0) {
            printf("error in accept\n");
            exit(1);
        }
        // creat acception process
        if ((pid = fork()) == 0) {
            close(sockfd);
            // ===== step 5: recv() the packet and response =====
            str_ser(con_fd);  // receive packet and response
            close(con_fd);
            exit(0);
        } else
            close(con_fd);  // parent process
    }
    // ===== step 6: close() socket =====
    close(sockfd);
    exit(0);
}

void str_ser(int sockfd) {
    char buf[BUFSIZE];
    FILE *fp;
    struct pack_so recvs;
    struct ack_so ack;
    int end, n = 0, ci, lsize = 1;
    ci = end = ack.num = 0;

    while (ci < lsize) {
        // receive the whole file from client
        if ((n = recv(sockfd, &recvs, MAXSIZE, 0)) == -1) {
            printf("receiving error!\n");
            return;
        } else
            printf("%d data received\n", n);
        if (ci == 0) {
            lsize = recvs.len;                       // copy the data length
            memcpy(buf, recvs.data, (n - HEADLEN));  // copy the data
            ci += n - HEADLEN;
        } else {
            memcpy((buf + ci), &recvs, n);
            ci += n;
        }
    }
    // send ack to client after receiving the whole file
    ack.len = 0;
    ack.num = 1;
    //	memcpy(buf, recvs.data, recvs.len);
    send(sockfd, &ack, 2, 0);
    // open new file to save received .txt file
    if ((fp = fopen("myTCPreceive.txt", "wt")) == NULL) {
        printf("File doesn't exit\n");
        exit(0);
    }
    printf("the data received: %d\n", ci);
    printf("the file size received: %d\n", lsize);
    fwrite(buf, 1, lsize, fp);  // write the data into file
    fclose(fp);
    printf("a file has been successfully received!\n");
}
