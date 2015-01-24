#ifndef XMPP_IO_SOCKET_HH_
#define XMPP_IO_SOCKET_HH_
#pragma once

#include "env.hh"
#include "variant.hh"

#include <list>
#include <map>
#include <vector>
#include <string>
#include <initializer_list>
#include <sstream>

#include <openssl/ssl.h> // NOTE: dtls1.h, s/winsock.h/winsock2.h/

#ifdef PLATFORM_WIN
# ifdef _WIN32_WINNT
#  undef _WIN32_WINNT
# endif
# define _WIN32_WINNT 0x501

# ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
# endif

# include <windows.h>
# include <winsock2.h>
# include <ws2tcpip.h>

# include "inet_pton.hh"
# include "inet_ntop.hh"
#else
# error "Not implemented yet."
#endif

namespace xmpp {

  class SocketAddr;

# ifdef PLATFORM_WIN
  using socket_t = SOCKET;
# endif

  using port_t = unsigned short;
  using SocketAddrList = std::list<SocketAddr>;

  class Sockets;
  class SocketAddr
  {
    friend class Sockets;

    public:
      enum Version
      {
        Ver4 = 1,
        Ver6 = 2,

        Both = Ver4|Ver6,
      };

    public:
      SocketAddr();
      SocketAddr(char* host_addr, port_t port, Version ver = Ver4, bool translated = false);
      ~SocketAddr();
    
    public:
      sockaddr_in6& get_v6();
      sockaddr_in& get_v4();
      sockaddr* get_generic();

      Version get_version() const;

    public:
      std::string get_addr() const;
      std::string get_name() const;

    protected:
      port_t port;

      union {
        sockaddr_in v4;
        sockaddr_in6 v6;
      } addr;

      Version ver;
  } /* class SocketAddr */;

  class Socket : public std::stringbuf
  {
    public:
      enum Protocol
      {
        TCP,
        UDP,
      };

      enum Mode
      {
        Client,
        Listener,
      };

      enum Option
      {
        OptTimeout,
      };

    private:
      using OptionValue = std::map<unsigned, Variant>::value_type;

    public:
      Socket(Protocol p = Socket::Protocol::TCP);
      virtual ~Socket();

    public:
      virtual bool setup(SocketAddr& addr, Mode mode = Socket::Mode::Client);
      virtual void close();

      virtual bool established();

    public:
      virtual bool accept(Socket& socket);
      virtual size_t recv();
      virtual size_t send(std::iostream& data);

    public:
      std::iostream& get_input_stream();

      const SocketAddr& get_addr();
      const SocketAddr& get_local_addr();

    public:
      virtual void set_options(std::initializer_list<OptionValue> o);
      Variant& get_option(unsigned k);

    protected:
      Protocol proto;
      SocketAddr addr_src, addr_dst;
      socket_t desc;

      std::iostream stream_in;

      std::map<unsigned, Variant> opts;
  };

  class SSLSocket : public Socket
  {
    public:
      SSLSocket(Protocol p = Socket::Protocol::TCP);
      ~SSLSocket();

    public:
      bool setup(SocketAddr& addr, Mode mode = Socket::Mode::Client);
      void close();

    public:
      bool accept(SSLSocket& socket);
      size_t recv();
      size_t send(std::iostream& data);
    
    private:
      SSL_CTX *ssl_ctx;
      SSL *ssl_handle;
  } /* class SSLSocket */;

  /**
    * SOCKETSERVICES
    *
    * Needed to resolve hostnames and setup platform independent libraries such as winsock
    */
  class SocketServices
  {    
    private:
      SocketServices();
      ~SocketServices();
    
    public:
      static SocketServices* get_instance();

    public:
      static void init_ssl();

    public:
      SocketAddrList resolve(const std::string& host, port_t port, SocketAddr::Version = SocketAddr::Version::Both, Socket::Protocol = Socket::Protocol::TCP);

    private:
#     ifdef PLATFORM_WIN
      WSADATA wsa_data;
#     endif
  } /* class SocketServices */;

}

#endif /* XMPP_IO_SOCKET_HH_ */
