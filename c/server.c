/**
 * @file server.c
 * @brief A simple daemon listening on a TCP port and performing tasks.
 * 
 * This daemon runs until an error occurs or until it observes a SIGINT.
 *
 * @todo: The server opens a file in the current working directory for logging.
 *        Instead, use system logger to avoid touching part of the filesystem from
 *        which the process started.
 *
 * @author Karel Dolezal, akwky@centrum.cz
 */

#define _POSIX_SOURCE 

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/wait.h> 
#include <fcntl.h>

#include "common.h"
#include "tasks.h"


// The buffer size for new TCP connections listen()
#define BACKLOG_SIZE   3
// Data buffer size for TCP communication. Used for both receive and send.
// The entire request/response content must fit into this buffer.
#define BUFFER_SIZE   80
// Default response for unknown requests.
#define RESPONSE_INVALID_REQUEST "Invalid request\n"

// Path to the log file
#define LOG_FILE "server.log"


// This variable is set by a signal handler.
int signalCaught = 0;

/**
 * @brief Signal handler for stopping the daemon nicely.
 * A global variable is set to indicate that a request was made to stop the daemon.
 *
 * @param signal This value is ignored
 */
void sigIntHandler(int signal)
{
  signalCaught = 1;
}

/**
 * @brief Switches the process to background
 *
 * Makes init the daemon's parent process and leaves the old process group
 * to avoid the group affecting it. Second fork is done to prevent the possibility
 * of the process acquiring a controling terminal (should not be necessary). 
 * Sets the working directory to root.
 *
 * All standard streams are redirected to a log file.
 */
void runAsDaemon()
{
  // re-parent to init
  int child = fork();
  if (child < 0) {
    die("fork()", ErrProcess);
  }
  if (child > 0) {
    int status;
    waitpid(child, &status, 0);
    exit(ErrOK);
  }
  
  // leave the old process group (makes the process a session leader)
  if (setsid() < 0) {
    die("setsid()", ErrProcess);
  }

  // fork again - cease to be the session leader
  child = fork();
  if (child < 0) {
    die("fork()", ErrProcess);
  }
  if (child > 0) {
    exit(ErrOK);
  }

  // redirect std streams to the log file
  int log;
  if ((log = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644)) < 0) {
    die("open()", ErrFile);
  }
  dup2(log, STDIN_FILENO);
  dup2(log, STDOUT_FILENO);
  dup2(log, STDERR_FILENO);
  close(log);

  // leave the old working directory  
  if (chdir("/") < 0) {
    die("chdir()", ErrFile);
  }
  
  printf("%d: Daemon starting\n", getpid());
}

/**
 * @brief Serves a request on the given socket.
 *
 * Reads the request string and performs a desired operation. Then sends 
 * back a response and terminates the connection.
 *
 * @param socket The open socket to the client.
 */
void processRequest(int socket) 
{
  char buffer[BUFFER_SIZE];
  int size;
  
  // read the request
  size = recv(socket, &buffer, sizeof(buffer), 0);
  if (size < 0) {
    die("recv()", ErrNetwork);
  }
  shutdown(socket, SHUT_RD);
  

  // perform the requested task
  char *response = RESPONSE_INVALID_REQUEST;
  if (strncmp(buffer, CMD_CPU, strlen(CMD_CPU)) == 0) {
    printf("%d: Recognized CPU request\n", getpid());
    sprintf(buffer, "Current CPU usage is %d %%\n", (int) (taskGetCpuUsage() * 100 + 0.5));
    response = buffer;
  }
  if (strncmp(buffer, CMD_MEM, strlen(CMD_MEM)) == 0) {
    printf("%d: Recognized MEM request\n", getpid());
    sprintf(buffer, "Current memory usage is %ld kB\n", taskGetUsedMemoryKb());
    response = buffer;
  }

  // send the response and close connection
  size = strlen(response);
  if (send(socket, response, size, MSG_EOR) != size) {
    die("send()", ErrNetwork);
  }
  shutdown(socket, SHUT_WR);
  close(socket);
  
  printf("%d: Request handled, exiting.\n", getpid());
}

/**
 * @brief Starts a TCP server listening on the given port
 * 
 * Opens port on the localhost. Any incoming connections for a new process which handles
 * it. This function can return only if a signal is received to stop the server.
 *
 * @param port The listenin port of the server.
 */
void listenOnPort(int port)
{
  struct sockaddr_in serverAddress, peerAddress;
  
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0) {
    die("socket()", ErrNetwork);
  }
  
  // bind socket and start listening on the specified port
  serverAddress.sin_addr.s_addr = INADDR_ANY; 
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port);
  
  const int one = 1;
  if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) != 0) {
    die("setsockopt()", ErrNetwork);
  }
  if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) != 0) {
    die("bind()", ErrNetwork);
  }
  if (listen(serverSocket, BACKLOG_SIZE) < 0) {
    die("listen()", ErrNetwork);
  }
  printf("%d: Listening on port %d\n", getpid(), PORT);

  // accept connections until a signal is received  
  while (!signalCaught) {
    unsigned int size;    
    size = sizeof(peerAddress);
    int peerSocket = accept(serverSocket, (struct sockaddr *) &peerAddress, &size);
    if (signalCaught) {
      printf("%d: Caught signal, exiting.\n", getpid());
      break;
    }
    if (peerSocket < 0) {
      die("accept()", ErrNetwork);
    }
    
    // fork a new process for each request
    switch(fork()) {
      case 0:
        printf("%d: Processing a new connection\n", getpid());
        close(serverSocket);
        processRequest(peerSocket);
        exit(ErrOK);

      case -1:
        close(serverSocket);
        die("fork()", ErrProcess);

      default:
        break;
    }
  }
}

/**
 * @brief Configure signal handling for the daemon.
 *
 * Sets the SIGINT handler to terminate the daemon nicely. Also ignore
 * return codes of the children processes, avoiding zombies.
 */
void setupSignals()
{
  struct sigaction action;

  // Register the Int signal
  bzero(&action, sizeof(struct sigaction));
  action.sa_handler = sigIntHandler;
  sigemptyset(&action.sa_mask);
  if (sigaction(SIGINT, &action, NULL) < 0) {
    die("sigaction()", ErrSignal);
  }
  
  // Make the daemon ignore exit status of its children
  bzero(&action, sizeof(struct sigaction));
  action.sa_handler = SIG_IGN;
  sigemptyset(&action.sa_mask);
  if (sigaction(SIGCHLD, &action, NULL) < 0) {
    die("sigaction()", ErrSignal);
  }
}

/**
 * @brief Starts the daemon.
 */
int main()
{
  printf("%d: Server starting\n", getpid());
  
  runAsDaemon();
  setupSignals();
  listenOnPort(PORT);

  return ErrOK;
}
