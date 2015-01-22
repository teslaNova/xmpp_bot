#include <iostream>

#include "socket.hh"

const char* HOST = "www.google.de";
const short PORT = 80;

int main(int argc, char const *argv[])
{
  xmpp::Socket http_connection;
  xmpp::SocketAddrList hosts = xmpp::SocketServices::get_instance()->resolve(HOST, PORT, xmpp::SocketAddr::Version::Ver6);

  if(hosts.empty())
  {
    std::cout << "Unable to resolve hostname '" << HOST << "'." << std::endl;
    return 1;
  }

  std::cout << "Host adresses found for '" << HOST << "': " << hosts.size() << std::endl;

  for(auto h : hosts)
  {
    std::cout << "Trying '" << h.get_addr() << "' .." << std::endl;

    if(true == http_connection.setup(h))
    {
      http_connection.set_options({
        {xmpp::Socket::OptBufSizeIn, (2 << 11)}
      });

      break;
    }
  }

  if(http_connection.established() == true)
  {
    std::cout << "TCP connection established with '" << http_connection.get_addr().get_name()
              << "' (as " << http_connection.get_addr().get_addr() << ")"
              << " on port " << PORT << std::endl;

    std::iostream socket_stream{&http_connection};
    socket_stream << "GET /\r\nHTTP/1.1\r\n\r\n";

    http_connection.send();
    
    if(0 < http_connection.recv_all())
    {
      std::cout << "Data received: ";

      while(socket_stream.rdbuf()->in_avail())
        std::cout << (char) socket_stream.get();
    }
  }
  else
  {
    std::cout << "Unable to setup a connection with '" << HOST 
              << "' on port" << PORT << " (resolved "
              << hosts.size() << " addr.) or version is below 6." << std::endl;
  }

  return 0;
}