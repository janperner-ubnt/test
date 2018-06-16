/**
 * @file crp.hpp
 * @brief Client-side request processor.
 *
 * Instance can handle multiple requests during its lifetime.
 *
 * @author Karel Dolezal, akwky@centrum.cz
 */
 
#ifndef _CRP_HPP_
#define _CRP_HPP_

#include <iostream>
#include <boost/asio.hpp>

/**
 * Client-side request processor.
 * Connects to a specified server, sends given request and retrieves response.
 */
class ClientRequestProcessor
{
public:
  ClientRequestProcessor(boost::asio::io_service *ioService);
 
  void process(std::ostream &output, std::string host, std::string command);
  
private:
  boost::asio::io_service *v_ioService;
  boost::asio::ip::tcp::resolver v_resolver;
  
  boost::asio::ip::tcp::endpoint resolveHostname(std::string host);
};

#endif