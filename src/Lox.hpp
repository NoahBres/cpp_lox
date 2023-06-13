#pragma once

#include "Parser.hpp"
#include "Scanner.hpp"
#include <fstream>
#include <iostream>
#include <string>

namespace lox {

class Lox {
  bool had_error = false;

public:
  auto run(const std::string &source) -> void {
    auto scanner = lox::Scanner{source};
    auto tokens = scanner.scanTokens();
    auto parser = lox::Parser{tokens};
    auto expression = parser.parse();

    if (had_error || !expression)
      return;

    // std::cout << lox::AstPrinter{}.print(expression.value()) << '\n';
  }

  auto runFile(const std::string &filePath) -> void {
    std::ifstream input{filePath};
    if (!input) {
      std::cerr << "Could not open file: " << filePath << '\n';
    }

    run(std::string{std::istreambuf_iterator<char>{input}, {}});
    if (had_error)
      exit(65);
  }

  auto runPrompt() -> void {
    std::string line;

    while (true) {
      std::cout << "> ";
      if (!std::getline(std::cin, line))
        break;

      run(line);
      had_error = false;
    }
  }
};

} // namespace lox