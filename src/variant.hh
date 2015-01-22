#ifndef _XMPP_VARIANT_HH
#define _XMPP_VARIANT_HH
#pragma once

namespace xmpp
{
  class Variant
  {
    public:
      Variant(bool);
      Variant(char);
      Variant(short);
      Variant(int);
      Variant(long);
      Variant(float);
      Variant(double = 0.0f);

      ~Variant();

    public:
      template<typename T> void set(T value);
      template<typename T> T get();
    
    private:
      union
      {
        bool bv;
        char cv;
        short sv;
        int iv;
        long lv;
        float fv;
        double dv;
      } value;
  } /* class Variant */;
} /* ns xmpp */;

#include "variant.cc.inl"

#endif /* _XMPP_VARIANT_HH */