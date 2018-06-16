/**
 * @file tasks.c
 * @brief This module implements the tasks supported by the daemon.
 *
 * @author Karel Dolezal, akwky@centrum.cz
 */
 
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "tasks.h"

// Note: The format's string length limitation must take into account
//       the total size of the buffer defined in LINE_BUFFER_SIZE
#define MEMINFO_FORMAT   "%79s %ld kB"
// Size of the line buffer used when reading files
#define LINE_BUFFER_SIZE 80

// Interesting keys in /proc/meminfo
#define MEM_KEY_TOTAL    "MemTotal:"
#define MEM_KEY_FREE     "MemFree:"
#define MEM_KEY_BUFFERS  "Buffers:"
#define MEM_KEY_CACHED   "Cached:"

// CPU usage measurement period
// Make the interval very long so it is noticeable (for demonstration purposes)
#define MEASUREMENT_INTERVAL_US 1000000

/**
 * @brief Retrieves information about current memory usage.
 *
 * Parses the /proc/meminfo file and outputs the number of kB currently used.
 * Note: current implementation does not expect a malformed meminfo file
 *
 * @returns The number of kB currently used on the machine.
 */
long taskGetUsedMemoryKb()
{
  long result = 0;
  
  // open the file
  FILE *file = fopen("/proc/meminfo", "r");
  if (!file) {
    die("fopen()", ErrFile);
  }
  
  // iterate through all the lines
  char *line = NULL;
  size_t size = 0;
  while (getline(&line, &size, file) > 0) {
    if (line == NULL) {
      fclose(file);
      die("getline()", ErrFile);
    }

    // parse the line as key value pair
    char key[LINE_BUFFER_SIZE];
    long value;
    if (sscanf(line, MEMINFO_FORMAT , key, &value) != 2) {
      free(line);
      line = NULL;
      continue;
    }
    
    // use interesting keys for the computation
    if (strcmp(key, MEM_KEY_TOTAL) == 0) {
      result += value;
    }
    if (strcmp(key, MEM_KEY_FREE) == 0 ||
        strcmp(key, MEM_KEY_BUFFERS) == 0 ||
        strcmp(key, MEM_KEY_CACHED) == 0) 
    {
      result -= value;
    }
    
    free(line);
    line = NULL;
  }
  free(line);
 
  fclose(file);
  return result;
}

/**
 * @brief Retrieve the cpu time spent "so far" 
 *
 * @param timeWorking The amount of time spent on processes will be stored here.
 * @param timeIdle The amount of idle time will be stored here.
 */
void getCpuUsage(long *timeWorking, long *timeIdle)
{
  FILE *file = fopen("/proc/stat", "r");
  if (!file) {
    die("fopen()", ErrFile);
  }
  
  char *line = NULL;
  size_t size = 0;
  
  // the total "cpu" statistics is always on the first line
  getline(&line, &size, file);
  if (line == NULL) {
    fclose(file);
    die("getline()", ErrFile);
  }
  fclose(file);
  
  // parse the "cpu" line
  long user, nice, system, idle, iowait, irq, softIrq, steal;
  if (sscanf(line, "cpu %ld %ld %ld %ld %ld %ld %ld %ld",
    &user, &nice, &system, &idle, &iowait, &irq, &softIrq, &steal) < 8) 
  {
    free(line);
    die("sscanf()", ErrFile);
  }
  free(line);
  
  // sum it up
  *timeWorking = user + nice + system + irq + softIrq + steal;
  *timeIdle = idle + iowait;
}

float taskGetCpuUsage()
{
  long workingA, idleA;
  long workingB, idleB;

  // retrieve usage in two points in time
  getCpuUsage(&workingA, &idleA);
  usleep(MEASUREMENT_INTERVAL_US);
  getCpuUsage(&workingB, &idleB);
  
  // calculate the delta
  long deltaTimeWorking = workingB - workingA;
  long deltaTimeTotal = (workingB + idleB) - (workingA + idleA);
  
  return (float) deltaTimeWorking / deltaTimeTotal;
}