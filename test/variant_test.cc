#include "variant.hh"

#include <iostream>

int main(int argc, char const *argv[])
{
  auto foo = xmpp::Variant{true};

  std::cout << "foo<bool>:  " << foo.get<bool>() << std::endl
            << "foo<int>:   " << foo.get<int>() << std::endl
            << "foo<float>: " << foo.get<float>() << std::endl
            << std::endl;

  foo.set<float>(1.23f);

  std::cout << "foo<bool>:  " << foo.get<bool>() << std::endl
            << "foo<int>:   " << foo.get<int>() << std::endl
            << "foo<float>: " << foo.get<float>() << std::endl
            << std::endl;

  return 0;
}