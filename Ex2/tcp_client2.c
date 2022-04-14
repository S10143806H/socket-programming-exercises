/*******************************
tcp_client.c: the source file of the client in tcp transmission for a large
packet
********************************/

#include "headsock.h"

// packet transmission fuction
float str_cli(FILE *fp, int sockfd, long *len);
// calcu the time interval between out and in
void tv_sub(struct timeval *out, struct timeval *in);

int main(int argc, char **argv) {
    int sockfd, ret;
    float ti, rt;
    long len;
    struct sockaddr_in ser_addr;
    char **pptr;
    struct hostent *sh;
    struct in_addr **addrs;
    FILE *fp;

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
    // open local file in read mode
    if ((fp = fopen("myfile.txt", "r+t")) == NULL) {
        printf("File doesn't exit\n");
        exit(0);
    }

    // ===== step 3: send() file to server side =====
    ti = str_cli(fp, sockfd, &len);
    if (ti != -1) {
        // caculate the average transmission rate
        rt = (len / (float)ti);
        printf(
            "Ave Time(ms) : %.3f, Ave Data sent(byte): %d\nAve Data rate: %f "
            "(Kbytes/s)\n",
            ti, (int)len, rt);
    }

    // ===== step 4: close() socket =====
    close(sockfd);
    fclose(fp);
    exit(0);
}

float str_cli(FILE *fp, int sockfd, long *len) {
    long lsize;
    struct pack_so sends;
    struct ack_so acks;
    int n;
    float time_inv = 0.0;
    struct timeval sendt, recvt;

    // determine the length of the file stored in fp
    fseek(fp, 0, SEEK_END);
    *len = lsize = ftell(fp);
    rewind(fp);
    printf("The file length is %d bytes\n", (int)lsize);

    // copy the file data into the buffer sends.data
    fread(sends.data, 1, lsize, fp);

    /*** the whole file is loaded in the buffer. ***/

    gettimeofday(&sendt, NULL);  // get the current time

    // send the file to the server
    sends.len = lsize;  // the data length
    sends.num = 0;
    // send the data as one chunk
    n = send(sockfd, &sends, (sends.len + HEADLEN), 0);
    if (n == -1) {
        printf("error sending data\n");
        exit(1);
    } else
        printf("%d data sent", n);

    // receive ACK or NACK
    if ((n = recv(sockfd, &acks, 2, 0)) == -1) {
        printf("error receiving data\n");
        exit(1);
    }
    // if it is ACK
    if ((acks.len == 0) && (acks.num == 1)) {
        gettimeofday(&recvt, NULL);  // get current time
        tv_sub(&recvt, &sendt);      // get the whole trans time
                                     // conver time_inv to ms
        time_inv += (recvt.tv_sec) * 1000.0 + (recvt.tv_usec) / 1000.0;
        return (time_inv);
    } else {
        printf("Error in transmission\n");
        return (-1);
    }
}

void tv_sub(struct timeval *out, struct timeval *in) {
    if ((out->tv_usec -= in->tv_usec) < 0) {
        --out->tv_sec;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}
