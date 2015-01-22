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

#ifdef PLATFORM_WIN
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
      SocketAddr(char* host_addr, port_t port, Version ver = Ver4);
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
        OptBufSizeOut,
        OptBufSizeIn,

        OptTimeout,
      };

    private:
      using OptionValue = std::map<unsigned, Variant>::value_type;

    public:
      Socket(Protocol p = Socket::Protocol::TCP);
      ~Socket();

    public:
      bool setup(SocketAddr& addr, Mode mode = Socket::Mode::Client);
      void close();

      bool established();

    public:
      bool accept(Socket& socket);
      size_t recv();
      size_t recv_all();
      size_t send();

    public:
      const SocketAddr& get_addr();
      const SocketAddr& get_local_addr();

    public:
      void set_options(std::initializer_list<OptionValue> o);
      Variant& get_option(unsigned k);

    private:
      Protocol proto;
      SocketAddr addr_src, addr_dst;
      socket_t desc;

      std::vector<char> buf_in, buf_out;

      std::map<unsigned, Variant> opts;
  };

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
      static SocketAddrList resolve(const std::string& host, port_t port, SocketAddr::Version = SocketAddr::Version::Both, Socket::Protocol = Socket::Protocol::TCP);


    private:
#     ifdef PLATFORM_WIN
      WSADATA wsa_data;
#     endif
  } /* class SocketServices */;

}

#endif /* XMPP_IO_SOCKET_HH_ */
