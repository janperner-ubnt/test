/**
 * @file args.cpp
 * @brief Command line argument helper.
 *
 * Preprocesses the arguments into a usable form.
 *
 * TODO: make the command map static
 *
 * @author Karel Dolezal, akwky@centrum.cz
 */

using namespace std;
 
#include <string>
#include <map>

#include "args.hpp"
#include "common.hpp"

/**
 * Creates the object with the knowledge of its source data.
 * @param argc Argument count.
 * @param argv Argument values.
 */
Arguments::Arguments(int argc, char *argv[])
{
  v_argc = argc;
  v_argv = argv;
}

/**
 * @brief Processes the data passed into constructor.
 * @returns True if the data was valid. Otherwise false.
 */
bool Arguments::parse()
{
  if (v_argc != 3) {
    return false;
  }

  // build a map for translating the commands 
  map<string, string> cmdMap;
  cmdMap.insert(make_pair("-c", CMD_CPU));
  cmdMap.insert(make_pair("-m", CMD_MEM));
  
  // translate the command switch to command string
  map<string, string>::iterator it = cmdMap.find(v_argv[2]);
  if (it == cmdMap.end()) {
    return false;
  }
  
  v_command = string(it->second);
  v_host = string(v_argv[1]);
  return true;
}

/**
 * @brief Returns the extracted host name.
 * @returns The name of the host to connect to.
 */
string Arguments::host() 
{
  return v_host;
}

/**
 * @brief Returns the extracted command.
 * @returns The command to be sent to the server.
 */
string Arguments::command()
{
  return v_command;
}

