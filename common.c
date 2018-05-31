/**
 * @file common.c
 * @brief Common functions used by both server and client.
 *
 * @author Karel Dolezal, akwky@centrum.cz
 */
 
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"

/**
 * @brief Prints an error message and exits.
 *
 * @param caller The name of the failing function.
 * @param code The exit code for the program.
 */
void die(char *caller, enum errorCode code) 
{
  fprintf(stderr, "%d: %s failed. %s\n", getpid(), caller, strerror(errno));
  exit(code);
}
