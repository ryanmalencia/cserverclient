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

#define USAGE                                                                 \
"usage:\n"                                                                    \
"  echoserver [options]\n"                                                    \
"options:\n"                                                                  \
"  -p                  Port (Default: 19121)\n"                                \
"  -m                  Maximum pending connections (default: 1)\n"            \
"  -h                  Show this help message\n"                              \

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
  {"port",          required_argument,      NULL,           'p'},
  {"maxnpending",   required_argument,      NULL,           'm'},
  {"help",          no_argument,            NULL,           'h'},
  {NULL,            0,                      NULL,             0}
};


int main(int argc, char **argv) {
  int option_char;
  int portno = 19121; /* port to listen on */
  int maxnpending = 1;
  int socketfd = 0, new_socket;
  int opt = 1, ret = 0;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[16];
  ssize_t recvsize, sendsize;
  
  // Parse and set command line arguments
  while ((option_char = getopt_long(argc, argv, "p:m:hx", gLongOptions, NULL)) != -1) {
   switch (option_char) {
      case 'p': // listen-port
        portno = atoi(optarg);
        break;                                        
      default:
        fprintf(stderr, "%s ", USAGE);
        exit(1);
      case 'm': // server
        maxnpending = atoi(optarg);
        break; 
      case 'h': // help
        fprintf(stdout, "%s ", USAGE);
        exit(0);
        break;
    }
  }

    setbuf(stdout, NULL); // disable buffering

    if ((portno < 1025) || (portno > 65535)) {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }
    if (maxnpending < 1) {
        fprintf(stderr, "%s @ %d: invalid pending count (%d)\n", __FILE__, __LINE__, maxnpending);
        exit(1);
    }


  /* Socket Code Here */
    socketfd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if(socketfd == -1) {
	fprintf(stderr, "Failed to create new socket");
        exit(1);
    }

    ret = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if(ret) {
	fprintf(stderr, "Unable to set socket options");
	exit(ret);
    }


    address.sin_family = PF_LOCAL;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(portno);
    ret = bind(socketfd, (struct sockaddr *)&address, sizeof(address));
    if(ret) {
        fprintf(stderr, "Failed to bind");
	exit(ret);
    }

    ret = listen(socketfd, 3);
    if(ret) {
	fprintf(stderr, "Failed to listen");
	exit(ret);
    }

    new_socket = accept(socketfd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    recvsize = recv(new_socket, buffer, 16, 0);
    if(recvsize < 0) {
	fprintf(stderr, "Error receiving message");
    }
    buffer[15] = 0;
    fprintf(stdout, "%s\n", buffer);
    sendsize = send(new_socket, buffer, 16, 0);
    if(sendsize != 16) {
	fprintf(stderr, "Error sending resposne");
    }
    return 0;
}
