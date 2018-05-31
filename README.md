# test
A very basic TCP Daemon

Use "make" command to build both client and server (daemon).

Start the server by "make run" or "./server", stop it by sending it SIGINT signal.
Start the client by "./client 127.0.0.1 -m" or run it without arguments to get usage info.

To kill the server, use "ps aux | grep server", or open "server.log" to find the PID.
Soft termination is possible using "kill -2 (pid)"

Tested on Debian 3.2.81-1
