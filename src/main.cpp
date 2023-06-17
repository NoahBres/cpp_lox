#include "Lox.hpp"
#include "Token.hpp"

#include <fstream>
#include <iostream>

auto main(int argc, char **argv) -> int {
  lox::Lox::run("1 + 3");

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
