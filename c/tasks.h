/**
 * @file tasks.h
 * @brief This module implements the tasks supported by the daemon.
 *
 * @author Karel Dolezal, akwky@centrum.cz
 */
 
#ifndef _TASKS_H_
#define _TASKS_H_

/**
 * @brief Retrieves information about current memory usage.
 *
 * @returns The number of kB currently used on the machine.
 */
long taskGetUsedMemoryKb();

/**
 * @brief Outputs the total CPU usage
 *
 * The usage is reported for all cores together, i.e. the value will be 0.25 on a quad
 * core cpu with one core fully used.
 * 
 * @returns Current CPU usage in the range 0 - 1.0
 */
float taskGetCpuUsage();

#endif