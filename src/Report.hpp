#pragma once

#include "Token.hpp"
#include <iostream>
#include <string>

namespace lox {
static void report(int line, const std::string &where,
                   const std::string &message) {
  std::cerr << "[line " << line << "] Error" << where << ": " << message
            << '\n';
}

static void error(int line, const std::string &message) {
  report(line, "", message);
}

static void error(const Token &token, const std::string &message) {
  if (token.type == TokenType::END_OF_FILE) {
    report(token.line, " at end", message);
  } else {
    report(token.line, " at '" + token.lexeme + "'", message);
  }
}

} // namespace lox