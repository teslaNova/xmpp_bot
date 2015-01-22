#include <type_traits>

namespace xmpp
{
  template<typename T> void Variant::set(T value)
  {
    this->value.dv = 0.;

    if(std::is_integral<T>::value == true)
    {
      this->value.lv = static_cast<T>(value);
    }
    else if(std::is_floating_point<T>::value == true)
    {
      this->value.dv = static_cast<T>(value);
    }
  }

  template<typename T> T Variant::get()
  {
    if(std::is_integral<T>::value == true)
    {
      return static_cast<T>(this->value.lv);
    }
    else if(std::is_floating_point<T>::value == true)
    {
      return static_cast<T>(this->value.dv);
    }

    return 0;
  }
} /* ns xmpp */;