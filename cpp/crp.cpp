/**
 * @file crp.cpp
 * @brief Client-side request processor.
 *
 * Instance can handle multiple requests during its lifetime.
 *
 * @author Karel Dolezal, akwky@centrum.cz
 */
 
#include <iostream>
#include <boost/asio.hpp>

#include "common.hpp"
#include "crp.hpp"

using namespace boost::asio;
using namespace std;

/**
 * Constructs the request processor
 * @param ioService An existing io_service instance for handling asio operations.
 */
ClientRequestProcessor::ClientRequestProcessor(io_service *ioService) 
  : v_resolver(*ioService)
{
  v_ioService = ioService;
}

/**
 * Sends request to the server and writes response to the given string
 * @param output Stream for writing the server's response
 * @param host Server hostname or address
 * @param command Command request
 */
void ClientRequestProcessor::process(ostream &output, string host, string command)
{
  // connect to the server
  ip::tcp::endpoint endpoint = resolveHostname(host);
  output << "Will connect to " << endpoint << endl;
  ip::tcp::socket socket(*v_ioService);
  socket.connect(endpoint);

  // write the request
  write(socket, boost::asio::buffer(command), boost::asio::transfer_all());

  // read the response
  boost::asio::streambuf buf;
  boost::system::error_code error;
  size_t len;
  while (len = read(socket, buf, transfer_all(), error) > 0) {
    output << boost::asio::buffer_cast<const char*>(buf.data());
  }
  if (error != boost::asio::error::eof) {
    // avoid exception handling during normal situations like EOF, but
    // throw on unexpected error code
    throw runtime_error(error.message());
  }

  socket.close();
}

/**
 * @brief Resolves the given host name.
 * @returns The endpoint.
 */
ip::tcp::endpoint ClientRequestProcessor::resolveHostname(string host)
{
  ip::tcp::resolver::query query(host, PORT);
  ip::tcp::resolver::iterator i = v_resolver.resolve(query);
  ip::tcp::resolver::iterator end;
  if (i == end) {
    throw runtime_error("Name not resolved.");
  }
  
  return *i;
}  
  
