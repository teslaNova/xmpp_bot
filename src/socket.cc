#include "socket.hh"

#include <cstring>
#include <tuple>

namespace xmpp {

  /**
    * SOCKETADDR
    */
  SocketAddr::SocketAddr()
  {
    memset(&this->addr, 0, sizeof(this->addr));
  }

  SocketAddr::SocketAddr(char* host_addr, port_t port, Version ver, bool translated)
    : port(port), ver(ver)
  {
    switch(ver)
    {
      case Ver4:
      {
        if(translated == true)
        {
          memcpy(&this->addr.v4.sin_addr, host_addr, sizeof(in_addr));
        }
        else
        {
          ::inet_pton4(host_addr, (unsigned char*)(&this->addr.v4.sin_addr));
        }

        this->addr.v4.sin_family = AF_INET;
        this->addr.v4.sin_port = htons(port);
      } break;

      case Ver6:
      {
        this->addr.v6.sin6_family = AF_INET6;
        this->addr.v6.sin6_port = htons(port);

        if(translated == true)
        {
          memcpy(&this->addr.v6.sin6_addr, host_addr, sizeof(in_addr6));
        }
        else
        {
          ::inet_pton6(host_addr, (unsigned char*)(&this->addr.v6.sin6_addr));
        }
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
      case Ver6: {
        char buf[64] = {0};
                
        if(nullptr != ::inet_ntop6((const unsigned char*)&this->addr.v6.sin6_addr, buf, 64))
        {
          tmp = buf;
        }
      } break;
    }

    return tmp;
  }  
  
   /**
    * SOCKET
    */
  Socket::Socket(Protocol p)
    : proto(p), desc(0), stream_in(nullptr)
  {
    this->set_options({
      {Option::OptTimeout, 120}
    });

    stream_in.rdbuf(&this->stream_in_buf);
  }

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
    if(this->desc != INVALID_SOCKET)
    {
#     ifdef PLATFORM_WIN
      closesocket(this->desc);
#     endif

      this->desc = INVALID_SOCKET;
    }
  }

  bool Socket::established()
  {
    return !(this->desc == INVALID_SOCKET);
  }

  bool Socket::accept(Socket& socket)
  {
    if(this->desc == INVALID_SOCKET)
    {
      return false;
    }

    return false;
  }

  size_t Socket::recv()
  {
    size_t read = 0;

    if(this->desc == INVALID_SOCKET)
    {
      return 0;
    }

    switch(this->proto)
    {
      case Protocol::TCP:
      {
        int buf_len;
        char buf[(2 << 6)] = {0};

        do
        {          
          read += buf_len = ::recv(this->desc, buf, (2 << 6) - 1, 0);
          this->stream_in.write(buf, buf_len);
        } while(buf_len > 0);
      } break;

      case Protocol::UDP:
      {

      } break;
    }
    
    return read;
  }

  size_t Socket::send(std::iostream& data)
  {
    size_t written = 0;

    if(this->desc == INVALID_SOCKET)
    {
      return 0;
    }

    switch(this->proto)
    {
      case Protocol::TCP:
      {
        while(data.rdbuf()->in_avail())
        {
          char buf[(2 << 6)] = {0};
          size_t buf_len = data.rdbuf()->in_avail() > (2 << 6) ? (2 << 6) : data.rdbuf()->in_avail();

          data.read(buf, buf_len);
          written += ::send(this->desc, buf, buf_len, 0);
        }
        
      } break;

      case Protocol::UDP:
      {

      } break;
    }

    return written;
  }

  std::iostream& Socket::get_input_stream()
  {
    return this->stream_in;
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

      switch(std::get<0>(v))
      {
        case Option::OptTimeout: {

        } break;
      }
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
    * SSLSOCKET
    */
  SSLSocket::SSLSocket(Protocol p)
    : Socket(p)
  {
    SocketServices::init_ssl();

    this->ssl_ctx = SSL_CTX_new(TLSv1_1_client_method());
    this->ssl_handle = SSL_new(this->ssl_ctx);
  }

  SSLSocket::~SSLSocket()
  {
    SSL_free(this->ssl_handle);
    SSL_CTX_free(this->ssl_ctx);
  }

  bool SSLSocket::setup(SocketAddr& addr, Mode mode)
  {
    if(Socket::setup(addr, mode) == false && this->ssl_handle != nullptr)
    {
      Socket::close();
      return false;
    }

    if(SSL_set_fd(this->ssl_handle, this->desc) == 0)
    {
      Socket::close();
      return false;
    }

    if(SSL_connect(this->ssl_handle) < 0 || SSL_do_handshake(this->ssl_handle) < 0)
    {
      Socket::close();
      SSL_shutdown(this->ssl_handle);
      return false;
    }

    return true;
  }

  void SSLSocket::close()
  {
    Socket::close();

    if(this->ssl_handle != nullptr) {
      while(SSL_shutdown(this->ssl_handle) >= 1); // TODO: ret == 0 check.
      SSL_clear(this->ssl_handle);
    }
  }

  bool SSLSocket::accept(SSLSocket& socket)
  {
    return false;
  }

  size_t SSLSocket::recv()
  {
    int read = 0;

    if(this->desc == INVALID_SOCKET)
    {
      return 0;
    }

    switch(this->proto)
    {
      case Protocol::TCP:
      {
        int buf_len;
        char buf[(2 << 6)] = {0};

        do
        {          
          read += buf_len = SSL_read(this->ssl_handle, buf, (2 << 6) - 1);
          this->stream_in.write(buf, buf_len);
        } while(buf_len > 0);
      } break;

      case Protocol::UDP:
      {

      } break;
    }
    
    return (read < 0 ? 0 : (size_t)read);
  }

  size_t SSLSocket::send(std::iostream& data)
  {
    int written = 0;

    if(this->desc == INVALID_SOCKET)
    {
      return 0;
    }

    switch(this->proto)
    {
      case Protocol::TCP:
      {
        while(data.rdbuf()->in_avail())
        {
          char buf[(2 << 6)] = {0};
          size_t buf_len = data.rdbuf()->in_avail() > (2 << 6) ? (2 << 6) : data.rdbuf()->in_avail();

          data.read(buf, buf_len);
          written += SSL_write(this->ssl_handle, buf, buf_len);
        }
        
      } break;

      case Protocol::UDP:
      {

      } break;
    }

    return (written < 0 ? 0 : (size_t)written);
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

  inline void SocketServices::init_ssl()
  {
    static bool is_ready = false;

    if(is_ready == false) {
      SSL_load_error_strings();
      SSL_library_init();

      is_ready = true;
    }
  }

  SocketAddrList SocketServices::resolve(const std::string& host, port_t port, SocketAddr::Version ip_ver, Socket::Protocol p)
  {
    SocketAddrList hosts;
    struct addrinfo *info, hints = {0};

    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = IPPROTO_TCP | IPPROTO_UDP;
    hints.ai_socktype = SOCK_STREAM | SOCK_DGRAM;

    switch(ip_ver)
    {
      case SocketAddr::Version::Ver4: hints.ai_family = AF_INET;
      case SocketAddr::Version::Ver6: hints.ai_family = AF_INET6;
      default: break;
    }

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
                hosts.push_back(SocketAddr{(char*)&((sockaddr_in*)info->ai_addr)->sin_addr, port, SocketAddr::Ver4, true});
              } break;

              case AF_INET6:
              {
                hosts.push_back(SocketAddr{(char*)&((sockaddr_in6*)info->ai_addr)->sin6_addr, port, SocketAddr::Ver6, true});
              } break;
            }
          }
        }

        info = info->ai_next;
      }
    }

    return hosts;
  }
} /* ns xmpp */;
