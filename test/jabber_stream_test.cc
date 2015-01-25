#include <iostream>

#include "client.hh"

const char* HOST = "127.0.0.1";
const short PORT = 5222;

int main(int argc, char const *argv[])
{
  xmpp::Client client; 

  std::cout << "Connecting with xmpp server " 
              << HOST << ":" << PORT << std::endl;

  if(!client.connect(HOST, PORT) == true)
  {
    std::cout << "Unable to connect to remote host" << std::endl;
  }

  return 0;
}