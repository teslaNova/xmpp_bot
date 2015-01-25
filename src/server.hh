#ifndef _XMPP_SERVER_HH
#define _XMPP_SERVER_HH
#pragma once

#include <map>

#include "variant.hh"

namespace xmpp
{  
  /* this class is only for information purposes */
  struct Server
  {
    enum Options
    {
      OptSASLMechanisms,
    };

    enum Features
    {
      FtSASLMD5 = 1,
      FtSASLSHA1 = 2,
    };

    std::map<unsigned, Variant> opts;
  } /* struct Server */;

} /* ns xmpp */;

#endif /* _XMPP_SERVER_HH */