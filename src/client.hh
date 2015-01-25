#ifndef _XMPP_CLIENT_HH
#define _XMPP_CLIENT_HH
#pragma once

#include "socket.hh"

//#include "eventhandler.hh"
#include "streamhandler.hh"

#include "server.hh"

#include <memory>

namespace xmpp {
  
  /*
    TODO: 
      - JID
  */
  class Client
  {
    friend class StreamHandler;

    public:
      Client();
      ~Client();
    
    public:
      // TODO: determine if host is name, addr or binary. currently assuming addr.
      bool connect(const char* host, port_t port);
      void disconnect();

    public:
      void run_once();
      void run();

    protected:
      Socket con;

      std::unique_ptr<StreamHandler> stream_handler;
      //std::unique_ptr<EventHandler> *event_handler;

    private:
      //JID jid;
      Server server;

    public:
      //static std::map<JID, Client*> clients;
  } /* class Client */;

} /* ns xmpp */;

#endif /* _XMPP_CLIENT_HH */