#include <iostream>

#include "socket.hh"

const char* HOST = "www.google.de";
const short PORT = 80;

int main(int argc, char const *argv[])
{
  xmpp::Socket http_connection;
  xmpp::SocketAddrList hosts = xmpp::SocketServices::get_instance()->resolve(HOST, PORT);

  if(hosts.empty())
  {
    std::cout << "Unable to resolve hostname '" << HOST << "'." << std::endl;
    return 1;
  }

  for(auto h : hosts)
  {
    std::cout << "Trying '" << h.get_addr() << "' .." << std::endl;

    if(true == http_connection.setup(h))
    {
      break;
    }
  }

  if(http_connection.established() == true)
  {
    std::cout << "TCP connection established with '" << http_connection.get_addr().get_name()
              << "' (as " << http_connection.get_addr().get_addr() << ")"
              << " on port " << PORT << std::endl;
  }
  else
  {
    std::cout << "Unable to setup a connection with '" << HOST 
              << "' on port" << PORT << " (resolved "
              << hosts.size() << " addr.)" << std::endl;
  }

  return 0;
}