#ifndef _INET_PTON_HH
#define _INET_PTON_HH
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

int inet_pton(int af, const char *src, void *dst);
int inet_pton4(const char *src, unsigned char *dst);
int inet_pton6(const char *src, unsigned char *dst);
#endif

#endif /* _INET_PTON_HH */