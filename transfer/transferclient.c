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
#include <fcntl.h>

#define BUFSIZE 1219

#define USAGE                                                \
    "usage:\n"                                               \
    "  transferclient [options]\n"                           \
    "options:\n"                                             \
    "  -s                  Server (Default: localhost)\n"    \
    "  -p                  Port (Default: 19121)\n"           \
    "  -o                  Output file (Default cs6200.txt)\n" \
    "  -h                  Show this help message\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
    {"server", required_argument, NULL, 's'},
    {"port", required_argument, NULL, 'p'},
    {"output", required_argument, NULL, 'o'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}};

/* Main ========================================================= */
int main(int argc, char **argv)
{
    int option_char = 0;
    char *hostname = "localhost";
    unsigned short portno = 19121;
    char *filename = "cs6200.txt";
    int socketfd = 0, outputfd = 0;
    int ret = 0;
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    char buffer[1024];
    memset(buffer, 0, 1024*sizeof(char));
    ssize_t msgsize = 0;

    setbuf(stdout, NULL);

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "s:p:o:hx", gLongOptions, NULL)) != -1)
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
        case 'o': // filename
            filename = optarg;
            break;
        case 'h': // help
            fprintf(stdout, "%s", USAGE);
            exit(0);
            break;
        }
    }

    if (NULL == hostname)
    {
        fprintf(stderr, "%s @ %d: invalid host name\n", __FILE__, __LINE__);
        exit(1);
    }

    if (NULL == filename)
    {
        fprintf(stderr, "%s @ %d: invalid filename\n", __FILE__, __LINE__);
        exit(1);
    }

    if ((portno < 1025) || (portno > 65535))
    {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }

    /* Socket Code Here */
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd == -1) {
	fprintf(stderr, "Error creating socket");
	exit(socketfd);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    ret = connect(socketfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(ret) {
	fprintf(stderr, "Connect failed");
	exit(ret);
    }
    outputfd = open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    while( (msgsize = recv(socketfd, buffer, 1024, 0)) != 0 && outputfd != 0) {
	buffer[msgsize] = 0;
	write(outputfd, buffer, msgsize);
    }
    close(outputfd);
    return 0;
}
