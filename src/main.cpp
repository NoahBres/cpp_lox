#include "Lox.hpp"
#include "Token.hpp"

#include <fstream>
#include <iostream>

auto main(int argc, char **argv) -> int {
  lox::Lox::run("(print 1 + 3 * (4 + 8); print \"yuhh\"; print true;)");
  std::cout << "\n\n--------------------------\n";
  lox::Lox::run(R"(var a = 1; print a;)");
  std::cout << "\n\n--------------------------\n";
  lox::Lox::run(R"(var a = "global a";
var b = "global b";
var c = "global c";
{
  var a = "outer a";
  var b = "outer b";
  {
    var a = "inner a";
    print a;
    print b;
    print c;
  }
  print a;
  print b;
  print c;
}
print a;
print b;
print c;
)");

  // if (argc > 2) {
  //   std::cout << "Usage: cpp_lox [script]\n";
  //   return 64;
  // } else if (argc == 2) {
  //   lox::Lox::runFile(argv[1]);
  // } else {
  //   lox::Lox::runPrompt();
  // }

  return 0;
}
