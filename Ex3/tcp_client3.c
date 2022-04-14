/*******************************
tcp_client.c: the source file of the client in tcp transmission
********************************/

#include "headsock.h"

// transmission function
float str_cli(FILE *fp, int sockfd, long *len);
// calcu the time interval between out and in
void tv_sub(struct timeval *out, struct timeval *in);

int main(int argc, char **argv) {
    int sockfd, ret;
    struct sockaddr_in ser_addr;
    char **pptr;
    struct hostent *sh;
    struct in_addr **addrs;
    float ti, rt;
    long len;
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
    // *** open local file in read mode ***
    if ((fp = fopen("myfile.txt", "r+t")) == NULL) {
        printf("File doesn't exit\n");
        exit(0);
    }

    // *** ===== step 3: send() data to server side ===== ***
    ti = str_cli(fp, sockfd, &len);  // perform the transmission and receiving
    rt = (len / (float)ti);          // caculate the average transmission rate
    printf("Time(ms) : %.3f, Data sent(byte): %d\nData rate: %f (Kbytes/s)\n",
           ti, (int)len, rt);

    // ===== step 4: close() socket =====
    close(sockfd);
    fclose(fp);
    exit(0);
}

float str_cli(FILE *fp, int sockfd, long *len) {
    char *buf;
    long lsize, ci;
    char sends[DATALEN];
    struct ack_so ack;  // client wait for ACK
    int n, slen;
    float time_inv = 0.0;
    struct timeval sendt, recvt;
    ci = 0;

    // determine the length of the file stored in fp
    fseek(fp, 0, SEEK_END);
    printf("fseek(fp, 0, SEEK_END): %d\n", fseek(fp, 0, SEEK_END));
    lsize = ftell(fp);
    rewind(fp);
    printf("The file length is %d bytes\n", (int)lsize);
    printf("the packet length is %d bytes\n", DATALEN);

    // allocate memory to contain the whole file.
    buf = (char *)malloc(lsize);
    if (buf == NULL) {
        exit(2);
    }

    // copy the file into the buffer.
    fread(buf, 1, lsize, fp);

    /*** the whole file is loaded in the buffer. ***/

    buf[lsize] = '\0';           // append the end byte
    gettimeofday(&sendt, NULL);  // get the current time

    // send the data as small packet in lsize
    while (ci <= lsize) {
        if ((lsize + 1 - ci) <= DATALEN)
            slen = lsize + 1 - ci;
        else
            slen = DATALEN;
        memcpy(sends, (buf + ci), slen);
        n = send(sockfd, &sends, slen, 0);
        if (n == -1) {
            printf("send error!\n");  // send the data
            exit(1);
        }
        ci += slen;
    }
    // receive ACK or NACK
    if ((n = recv(sockfd, &ack, 2, 0)) == -1) {
        printf("error when receiving\n");
        exit(1);
    }
    // if NACK
    if (ack.num != 1 || ack.len != 0) {
        printf("error in transmission\n");
        return (-1);
    }
    gettimeofday(&recvt, NULL);
    *len = ci;               // get current time
    tv_sub(&recvt, &sendt);  // get the whole trans time
    time_inv += (recvt.tv_sec) * 1000.0 + (recvt.tv_usec) / 1000.0;
    return (time_inv);
}

void tv_sub(struct timeval *out, struct timeval *in) {
    if ((out->tv_usec -= in->tv_usec) < 0) {
        --out->tv_sec;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}
