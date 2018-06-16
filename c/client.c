/**
 * @file client.c
 * @brief A simple client for sending requests to a TCP server.
 * 
 * Connects to a given address and sents specified request. The response is 
 * printed to a standard output.
 *
 * @author Karel Dolezal, akwky@centrum.cz
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#include "common.h"

// Size of the receive buffer. Can be any reasonable size.
#define RECV_BUFFER_SIZE 80
// Help text displayed in case of invalid arguments are specified.
#define USAGE "Usage: client <server> (-c | -m)\n"

// The supported requests as command line arguments.
#define OPTION_CPU "-c"
#define OPTION_MEM "-m"

/**
 * @Brief Checks the command line arguments.
 *
 * Prints usage and exits if the arguments are invalid.
 *
 * @param argc Argument count
 * @param argv Array of argument strings
 * @param server The server address is passed back through here
 * @param request The request string is passed back through here
 */
void processArguments(int argc, char *argv[], char **server, char **request)
{
  if (argc != 3) {
    printf(USAGE);
    exit(ErrArgs);
  }
  
  // translate cmd line options to the full command string
  *request = "\n";
  if (strcmp(argv[2], OPTION_CPU) == 0) {
    *request = CMD_CPU;
  }
  if (strcmp(argv[2], OPTION_MEM) == 0) {
    *request = CMD_MEM;
  }
  if ((*request)[0] == '\n') {
    printf(USAGE);
    exit(ErrArgs);
  }
  
  // the server adress string is simply passed "as is"
  *server = argv[1];
}

/**
 * @brief Returns socket connected to the given address and port.
 * Resolves the server address and opens a TCP connection.
 *
 * Note: Due to simplicity of this client, this function just exits on error.
 *
 * @param server The hostname or IP of the server.
 * @param port The port number of the server.
 * @returns Open socket.
 */
int openConnectionToServer(char *server, int port) 
{
  // resolve server hostname
  struct hostent *hptr = gethostbyname(server);
  if (!hptr) {
    printf(USAGE);
    die("gethostname()", ErrNetwork);
  }
  
  // fill in port and the address
  struct sockaddr_in address;
  memcpy(&address.sin_addr, hptr->h_addr_list[0], hptr->h_length);
  address.sin_family = AF_INET;
  address.sin_port = htons(port);

  // contact the server
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    die("socket()", ErrNetwork);
  }
  if (connect(sock, (struct sockaddr *) &address, sizeof(address)) != 0) {
    close(sock);
    die("connect()", ErrNetwork);
  }

  return sock;
}

/**
 * @brief Sends the given request to the provided socket, writes response to stdout.
 * The socket is shutdown for write after the request is sent and then closed after
 * the whole response is received.
 *
 * The response can be of any length.
 * Note: Due to simplicity of this client, this function just exits on error.
 *
 * @param sock The open socket.
 * @param request The request string.
 */
void processRequest(int sock, char *request) 
{
  // pass the request
  int requestLength = strlen(request);
  if (send(sock, request, requestLength, MSG_EOR) != requestLength) {
    close(sock);
    die("send()", ErrNetwork);
  }
  if (shutdown(sock, SHUT_WR) != 0) {
    close(sock);
    die("shutdown()", ErrNetwork);
  }
  
  // dump all received data to stdout as a response
  char buffer[RECV_BUFFER_SIZE];
  int size;
  while ((size = recv(sock, buffer, RECV_BUFFER_SIZE, 0)) > 0) {
    if (fwrite(buffer, 1, size, stdout) != size) {
      close(sock);
      die("fwrite()", ErrFile);
    }
  }
  if (size < 0) {
    die("recv()", ErrNetwork);
  }
  shutdown(sock, SHUT_RD);
  close(sock);
}

/**
 * @brief Starts the client.
 *
 * @param argc Two arguments are expected.
 * @param argv The first argument is the host name or IP address of the server,
 *             the second one must be one of the switches specified above.
 */
int main(int argc, char *argv[]) {
  
  char *server;
  char *request;
  
  processArguments(argc, argv, &server, &request);
  
  // process the request
  int sock = openConnectionToServer(server, PORT);
  processRequest(sock, request);
 
  return ErrOK;
}
