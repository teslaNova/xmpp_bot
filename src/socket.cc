#include "socket.hh"

#include <cstring>
#include <tuple>

#include <iostream>

namespace xmpp {

  /**
    * SOCKETADDR
    */
  SocketAddr::SocketAddr()
  {
    memset(&this->addr, 0, sizeof(this->addr));
  }

  SocketAddr::SocketAddr(char* host_addr, port_t port, Version ver)
    : port(port), ver(ver)
  {
    switch(ver)
    {
      case Ver4:
      {
        this->addr.v4.sin_family = AF_INET;
        this->addr.v4.sin_port = htons(port);
        
        ::inet_pton4(host_addr, (unsigned char*)(&this->addr.v4.sin_addr));
      } break;

      case Ver6:
      {
        this->addr.v6.sin6_family = AF_INET6;
        this->addr.v6.sin6_port = htons(port);

        ::inet_pton6(host_addr, (unsigned char*)(&this->addr.v6.sin6_addr));
        // TODO: implement inet_addr() equiv. for ipv6
      } break;
    }
  }

  SocketAddr::~SocketAddr()
  {

  }

  sockaddr_in& SocketAddr::get_v4()
  {
    return this->addr.v4;
  }

  sockaddr_in6& SocketAddr::get_v6()
  {
    return this->addr.v6;
  }

  sockaddr* SocketAddr::get_generic()
  {
    switch(this->ver)
    {
      case Ver4: return reinterpret_cast<sockaddr*>(&(this->addr.v4));
      case Ver6: return reinterpret_cast<sockaddr*>(&(this->addr.v6));
    }

    return nullptr;
  }

  SocketAddr::Version SocketAddr::get_version() const
  {
    return this->ver;
  }

  std::string SocketAddr::get_name() const
  {
    std::string host_addr{this->get_addr()};
    struct hostent *h_ent = nullptr;

    switch(this->ver)
    {
      case Ver4:
      {
        h_ent = gethostbyaddr((char*)(&this->addr.v4.sin_addr), 4, AF_INET);
      } break;;

      case Ver6:
      {
        h_ent = gethostbyaddr((char*)(&this->addr.v6.sin6_addr), 16, AF_INET6);
      } break;
    }

    if(h_ent == nullptr)
    {
      return {"<unresolved hostname>"};
    }

    return std::string{h_ent->h_name};
  }

  std::string SocketAddr::get_addr() const // TODO: implement ipv6
  {
    std::string tmp;

    switch(this->ver)
    {
      case Ver4: tmp = inet_ntoa(this->addr.v4.sin_addr); break;
      case Ver6: break;
    }

    return tmp;
  }  
  
   /**
    * SOCKET
    */
  Socket::Socket(Protocol p)
    : proto(p), desc(0)
  {}

  Socket::~Socket()
  {
    this->close();
  }

  bool Socket::setup(SocketAddr& addr, Mode mode)
  {
    this->desc = ::socket(addr.get_version() == SocketAddr::Ver4 ? AF_INET : AF_INET6,
                          this->proto == Protocol::TCP ? SOCK_STREAM : SOCK_DGRAM,
                          this->proto == Protocol::TCP ? IPPROTO_TCP : IPPROTO_UDP);

    if(this->desc == INVALID_SOCKET)
    {
      return false;
    }

    switch(mode)
    {
      case Mode::Client:
      {
        this->addr_dst = addr;

        if(SOCKET_ERROR == ::connect(this->desc, 
                              addr.get_generic(),
                              addr.get_version() == SocketAddr::Ver4 ? 
                                sizeof(sockaddr_in) : sizeof(sockaddr_in6)))
        {
          return false;
        }
      } break;

      case Mode::Listener:
      {
        this->addr_src = addr;
        // TODO: implement
      } break;
    }
    
    return true;
  }

  void Socket::close()
  {
    if(this->desc != 0)
    {
#     ifdef PLATFORM_WIN
      closesocket(this->desc);
#     endif

      this->desc = 0;
    }
  }

  bool Socket::established()
  {
    return !(this->desc == INVALID_SOCKET);
  }

  bool Socket::accept(Socket& socket)
  {
    return false;
  }

  size_t Socket::read()
  {
    return 0L;
  }

  size_t Socket::write()
  {
    return 0L;
  }

  const SocketAddr& Socket::get_addr()
  {
    return this->addr_dst;
  }

  const SocketAddr& Socket::get_local_addr()
  {
    return this->addr_src;
  }

  void Socket::set_options(std::initializer_list<OptionValue> o)
  {
    for(auto v : o)
    {
      this->opts[std::get<0>(v)] = std::get<1>(v);
    }
  }

  Variant& Socket::get_option(unsigned k)
  {
    static Variant null{(double)0.0f};

    try 
    {
      return this->opts.at(k);
    }
    catch(...)
    {
      return null;
    }
  }

  /**
    * SOCKETSERVICES
    */
  SocketServices::SocketServices()
  {
#   ifdef PLATFORM_WIN
    WSAStartup(MAKEWORD(2,2), &wsa_data);
#   endif
  }

  SocketServices::~SocketServices()
  {
#   ifdef PLATFORM_WIN
    WSACleanup();
#   endif
  }

  SocketServices* SocketServices::get_instance()
  {
    static SocketServices svc;
    return &svc;
  }

  SocketAddrList SocketServices::resolve(const std::string& host, port_t port, Socket::Protocol p)
  {
    SocketAddrList hosts;
    struct addrinfo *info, hints = {0};

    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = IPPROTO_TCP | IPPROTO_UDP;
    hints.ai_socktype = SOCK_STREAM | SOCK_DGRAM;

    if(0 == ::getaddrinfo(host.c_str(), nullptr, &hints, &info))
    {
      while(info != nullptr)
      {
        if(p == Socket::Protocol::TCP || p == Socket::Protocol::UDP)
        {
          if((info->ai_socktype & SOCK_STREAM && info->ai_protocol & IPPROTO_TCP)
          || (info->ai_socktype & SOCK_DGRAM && info->ai_protocol & IPPROTO_UDP))
          {
            switch(info->ai_family)
            {
              case AF_INET:
              {
                hosts.push_back(SocketAddr{inet_ntoa(((sockaddr_in*)info->ai_addr)->sin_addr), port, SocketAddr::Ver4});
              } break;

              case AF_INET6:
              {
                char buf[64] = {0};
                
                if(nullptr == ::inet_ntop(AF_INET6, (in_addr6*)&((sockaddr_in6*) info->ai_addr)->sin6_addr, buf, 63))
                {
                  continue;
                }

                hosts.push_back(SocketAddr{buf, port, SocketAddr::Ver4});
              } break;
            }
          }
        }

        info = info->ai_next;
      }
    }
  }
}
