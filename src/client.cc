#include "client.hh"

namespace xmpp {

  /**
    * CLIENT
    */
  Client::Client()
  {
    this->stream_handler.reset(new StreamHandler(reinterpret_cast<Client *>(this)));
  }

  Client::~Client()
  {
    this->disconnect();
  }

  bool Client::connect(const char* host, port_t port)
  {
    xmpp::SocketServices::get_instance(); // init

    SocketAddr addr{host, port};

    if(false == this->con.setup(addr))
    {
      return false;
    }

    // TODO:
    // - send initial stream.
    // - insert client to Client::clients

    this->stream_handler->startStream();
    this->stream_handler->endStream();

    return true;
  }

  void Client::disconnect()
  {
    // TODO: remove client from Client::clients
    return this->con.close();
  }

  void Client::run_once()
  {
    if(this->con.established() == false)
    {
      return;
    }
  }

  void Client::run()
  {
    while(this->con.established() == true)
    {
      this->run_once();
    }
  }

} /* ns xmpp */;
