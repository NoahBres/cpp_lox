#include "Lox.hpp"
#include "Token.hpp"

#include <fstream>
#include <iostream>

auto main(int argc, char **argv) -> int {
  lox::Lox::run("(print 1 + 3 * (4 + 8); print \"yuhh\"; print true;)");
  std::cout << "\n\n--------------------------\n";
  lox::Lox::run(R"(var a = 1; print a;)");

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
