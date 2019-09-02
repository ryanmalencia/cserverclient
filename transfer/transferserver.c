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

#define BUFSIZE 1219

#define USAGE                                                \
    "usage:\n"                                               \
    "  transferserver [options]\n"                           \
    "options:\n"                                             \
    "  -f                  Filename (Default: 6200.txt)\n" \
    "  -h                  Show this help message\n"         \
    "  -p                  Port (Default: 19121)\n"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
    {"filename", required_argument, NULL, 'f'},
    {"help", no_argument, NULL, 'h'},
    {"port", required_argument, NULL, 'p'},
    {NULL, 0, NULL, 0}};

int main(int argc, char **argv)
{
    int option_char;
    int portno = 19121;             /* port to listen on */
    char *filename = "6200.txt"; /* file to transfer */
    int socketfd = 0, new_socket = 0;
    int opt = 1, ret = 0;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024];
    ssize_t sendsize = 0;
    memset(&address, 0, sizeof(struct sockaddr_in));
    memset(buffer, 0, 1024*sizeof(char));

    setbuf(stdout, NULL); // disable buffering

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "p:hf:x", gLongOptions, NULL)) != -1)
    {
        switch (option_char)
        {
        case 'p': // listen-port
            portno = atoi(optarg);
            break;
        default:
            fprintf(stderr, "%s", USAGE);
            exit(1);
        case 'h': // help
            fprintf(stdout, "%s", USAGE);
            exit(0);
            break;
        case 'f': // file to transfer
            filename = optarg;
            break;
        }
    }


    if ((portno < 1025) || (portno > 65535))
    {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }
    
    if (NULL == filename)
    {
        fprintf(stderr, "%s @ %d: invalid filename\n", __FILE__, __LINE__);
        exit(1);
    }

    /* Socket Code Here */
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd == -1) {
        fprintf(stderr, "Failed to create new socket: %s\n", strerror(errno));
	exit(socketfd);
    }
    
    ret = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    if(ret) {
        fprintf(stderr, "Unable to set socket options: %s\n", strerror(errno));
	exit(ret);
    }

    ret = listen(socketfd, 3);
    if(ret) {
	fprintf(stderr, "Failed to listen: %s\n", strerror(errno));
	exit(ret);
    }
    
    while(1) {
	new_socket = accept(socketfd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
	ssize_t totalsent = 0; 
	sendsize = send(new_socket, buffer, 1024, 0);
	if(sendsize < 0) {fprintf(stderr, "Error receiving message\n");continue;};
        totalsent += sendsize;

    }
}
