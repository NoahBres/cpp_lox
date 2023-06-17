#pragma once

#include "Token.hpp"
#include <exception>
#include <iostream>
#include <optional>
#include <string>

namespace lox {
class ReportError : public std::exception {
private:
  auto inline formatError(int line, const std::string &where,
                          const std::string &message) {
    return "[line " + std::to_string(line) + "] Error" + where + ": " + message;
  }

public:
  std::optional<Token> const token;
  std::optional<int> const line;
  std::string message;

  ReportError(std::string message)
      : token{std::nullopt}, line{std::nullopt}, message{message} {}

  ReportError(int line, std::string message)
      : token{std::nullopt}, line{line}, message{message} {}

  ReportError(Token token, std::string message)
      : token{std::make_optional(token)}, line{token.line}, message{message} {};

  auto what() const noexcept -> const char * override {
    return message.c_str();
  }

  auto toString() {
    if (token.has_value()) {
      auto t = token.value();

      return formatError(t.line,
                         t.type == TokenType::END_OF_FILE
                             ? " at end"
                             : " at '" + t.lexeme + "'",
                         message);
    } else if (line.has_value()) {
      return "[line " + std::to_string(line.value()) + "] Error:" + message;
    } else {
      return message;
    }
  }
};

template <typename T> class Report {
public:
  T status;
  std::vector<ReportError> errors;

  Report(T status) : status{status} {};

  auto addError(ReportError error) { errors.push_back(error); }

  auto printErrors() {
    for (auto &err : errors) {
      std::cerr << err.toString() << std::endl;
    }
  }
};

} // namespace lox