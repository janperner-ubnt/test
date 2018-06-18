/**
 * @file client.cpp
 * @brief A simple client for sending requests to a TCP server
 *
 * Connects to a given address and sents specified request. The response is
 * printed to a standard output.
 *
 * @author Karel Dolezal, akwky@centrum.cz
 */

// Allowed libs: stl, boost (asio), pthread
#include <iostream>
#include <boost/asio.hpp>

#include "common.hpp"
#include "crp.hpp"
#include "args.hpp"

#define USAGE "Usage: client <server> (-c | -m)\n"

using namespace boost::asio;
using namespace std;

/**
 * @brief Program entry point
 * @returns Zero on success, otherwise a nonzero error code.
 */
int main(int argc, char *argv[]) {
  
  // @JP@ most of lines below can throw an exception, so the try-catch should cover entire main() function
  // process command line arguments
  Arguments args(argc, argv);
  if (!args.parse()) {
    cout << USAGE << endl;
    return ErrArgs;
  }
  
  // instantiate request processor
  io_service ioService;
  // @JP@ you could also register a signal handler into ioService, in order to interrupt a client waiting for server response

  ClientRequestProcessor client(&ioService);
  
  // process the request
  try {
    client.process(cout, args.host(), args.command());
  } 
  catch (const std::exception &ex) {
    cerr << "Exception: " << ex.what() << endl;
    return ErrGeneral;
  }
}
