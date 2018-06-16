/**
 * @file common.hpp
 * @brief Common functions used by both server and client.
 *
 * @author Karel Dolezal, akwky@centrum.cz
 */
 
#ifndef _COMMON_HPP_
#define _COMMON_HPP_

// All known commands supported by the server (and client).
#define CMD_CPU     "cpu\n"
#define CMD_MEM     "mem\n"

#define PORT        "5001"

/**
 * Specifies exit codes for the programs. 
 * Only general type of error is reported, see error message in the log for details.
 */
enum errorCode
{
  ErrOK = 0,
  ErrArgs,      // Invalid command line arguments
  ErrGeneral,   // General failure code (see exception text for more details)
};

#endif
