#ifndef _INET_NTOP_HH
#define _INET_NTOP_HH
#pragma once

#include "env.hh"

#ifdef PLATFORM_WIN
# ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
# endif

# include <windows.h>
# include <winsock2.h>
# include <ws2tcpip.h>

# define IN6ADDRSZ       16
# define INADDRSZ         4
# define INT16SZ          2

char *inet_ntop(int af, const void *src, char *buf, size_t size);
char *inet_ntop4 (const unsigned char *src, char *dst, size_t size);
char *inet_ntop6 (const unsigned char *src, char *dst, size_t size);
#endif

#endif /* _INET_NTOP_HH */