#include "Lox.hpp"
#include "Token.hpp"

#include <fstream>
#include <iostream>

auto main(int argc, char **argv) -> int {
  if (argc > 2) {
    std::cout << "Usage: cpp_lox [script]\n";
    return 64;
  } else if (argc == 2) {
    lox::Lox lox;
    lox.runFile(argv[1]);
  } else {
    lox::Lox lox;
    lox.runPrompt();
  }

  return 0;
}
