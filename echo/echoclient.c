#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <getopt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

/* Be prepared accept a response of this length */
#define BUFSIZE 1219

#define USAGE                                                                       \
    "usage:\n"                                                                      \
    "  echoclient [options]\n"                                                      \
    "options:\n"                                                                    \
    "  -s                  Server (Default: localhost)\n"                           \
    "  -p                  Port (Default: 19121)\n"                                  \
    "  -m                  Message to send to server (Default: \"Hello world.\")\n" \
    "  -h                  Show this help message\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
    {"server", required_argument, NULL, 's'},
    {"port", required_argument, NULL, 'p'},
    {"message", required_argument, NULL, 'm'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}};

/* Main ========================================================= */
int main(int argc, char **argv)
{
    int option_char = 0;
    char *hostname = "localhost";
    unsigned short portno = 19121;
    char *message = "Hello World!!!!";
    int socketfd;
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    struct addrinfo adinf;
    memset(&adinf, 0, sizeof(struct addrinfo));
    adinf.ai_family = AF_UNSPEC;
    adinf.ai_socktype = SOCK_STREAM;
    adinf.ai_flags = AI_PASSIVE;
    adinf.ai_protocol = 0;
    adinf.ai_canonname = NULL;
    adinf.ai_addr = NULL;
    adinf.ai_next = NULL;
    int ret;
    char buffer[16];
    ssize_t msgsize;

    struct addrinfo *result;
    getaddrinfo(NULL, hostname, &adinf, &result);

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "s:p:m:hx", gLongOptions, NULL)) != -1)
    {
        switch (option_char)
        {
        case 's': // server
            hostname = optarg;
            break;
        case 'p': // listen-port
            portno = atoi(optarg);
            break;
        default:
            fprintf(stderr, "%s", USAGE);
            exit(1);
        case 'm': // message
            message = optarg;
            break;
        case 'h': // help
            fprintf(stdout, "%s", USAGE);
            exit(0);
            break;
        }
    }

    setbuf(stdout, NULL); // disable buffering

    if ((portno < 1025) || (portno > 65535))
    {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }

    if (NULL == message)
    {
        fprintf(stderr, "%s @ %d: invalid message\n", __FILE__, __LINE__);
        exit(1);
    }

    if (NULL == hostname)
    {
        fprintf(stderr, "%s @ %d: invalid host name\n", __FILE__, __LINE__);
        exit(1);
    }

    /* Socket Code Here */
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd == -1) {
	fprintf(stderr, "Error creating socket");
	exit(1);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    /*if(inet_pton(PF_LOCAL, hostname, &serv_addr.sin_addr)<=0) {
        fprintf(stderr, "Invalid address");
        exit(1);
    }*/
    ret = connect(socketfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(ret) {
	fprintf(stderr, "Connect failed");
	exit(1);
    }
    //while(1) {

    msgsize = send(socketfd, message, strlen(message), 0);
    if(msgsize != strlen(message)) {
	fprintf(stderr, "Message sending error");
	exit(1);
    }

    msgsize = recv(socketfd, buffer, 16, 0);
    if(msgsize != 16) {
	fprintf(stderr, "Full message not received");
	exit(1);
    }
    char in[2];
    fscanf(stdin, "%s", in);
    if(in[0] == 'q') {
    }
    //}

    close(socketfd);
    return 0;
}
