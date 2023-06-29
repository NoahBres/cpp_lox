#pragma once

#include <exception>
#include <iostream>
#include <optional>
#include <string>
#include <utility>

#include "Token.hpp"

namespace lox {
  class ReportError : public std::exception {
  private:
    static auto inline formatError(int line, const std::string &where,
                                   const std::string &message) {
      return "[line " + std::to_string(line) + "] Error" + where + ": " +
             message;
    }

  public:
    std::optional<Token> const token;
    std::optional<int> const line;
    std::string message;

    ReportError(std::string message)
        : token{std::nullopt}, line{std::nullopt}, message{std::move(message)} {
    }

    ReportError(int line, std::string message)
        : token{std::nullopt}, line{line}, message{std::move(message)} {}

    ReportError(Token token, std::string message)
        : token{std::make_optional(token)}, line{token.line},
          message{std::move(message)} {};

    [[nodiscard]] auto what() const noexcept -> const char * override {
      return message.c_str();
    }

    auto toString() {
      if (token.has_value()) {
        auto val = token.value();

        return formatError(val.line,
                           val.type == TokenType::END_OF_FILE
                               ? " at end"
                               : " at '" + val.lexeme + "'",
                           message);
      }

      if (line.has_value()) {
        return "[line " + std::to_string(line.value()) + "] Error:" + message;
      }

      return message;
    }
  };

  template <typename T> class Report {
  public:
    T status;
    std::vector<ReportError> errors;

    Report(T status) : status{status} {};

    auto addError(const ReportError &error) { errors.push_back(error); }

    auto printErrors() {
      for (auto &err : errors) {
        std::cerr << err.toString() << std::endl;
      }
    }
  };

} // namespace lox