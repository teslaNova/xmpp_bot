#ifndef XMPP_ENV_HH_
#define XMPP_ENV_HH_
#pragma once

#if defined(_WIN32) || defined(_WIN32_) || defined(__WIN32__) || defined(__WIN32)
# define PLATFORM_WIN
#elif defined(linux) || defined(APPLE)
# define PLATFORM_POSIX
#else
# define PLATFORM_UNDETECTED
#endif

#endif /* XMPP_ENV_HH_ */
