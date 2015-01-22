#include "variant.hh"

namespace xmpp
{
  Variant::Variant(bool value)
  {
    this->set<bool>(value);
  }

  Variant::Variant(char value)
  {
    this->set<char>(value);
  }

  Variant::Variant(short value)
  {
    this->set<short>(value);
  }

  Variant::Variant(int value)
  {
    this->set<int>(value);
  }

  Variant::Variant(long value)
  {
    this->set<long>(value);
  }

  Variant::Variant(float value)
  {
    this->set<float>(value);
  }

  Variant::Variant(double value)
  {
    this->set<double>(value);
  }

  Variant::~Variant()
  {}

} /* ns xmpp */;