/**
 * @file common.h
 * @brief Common functions used by both server and client.
 *
 * @author Karel Dolezal, akwky@centrum.cz
 */
 
#ifndef _COMMON_H_
#define _COMMON_H_

// All known commands supported by the server (and client).
#define CMD_CPU     "cpu\n"
#define CMD_MEM     "mem\n"

#define PORT          5001

/**
 * Specifies exit codes for the programs. 
 * Only general type of error is reported, see error message in the log for details.
 */
enum errorCode
{
  ErrOK = 0,
  ErrProcess,   // Process handling like fork, setsid, etc
  ErrNetwork,   // Network operations like listen, bind, etc
  ErrSignal,    // Signal handling
  ErrArgs,      // Invalid command line arguments
  ErrFile,      // File operations like open, fwrite, etc.
};

/**
 * @brief Prints an error message and exits.
 *
 * @param caller The name of the failing function.
 * @param code The exit code for the program.
 */
void die(char *caller, enum errorCode code);

#endif