#pragma once

#include <any>
#include <string>
#include <unordered_map>

#include "Report.hpp"
#include "Token.hpp"

namespace lox {
  class Environment {
  private:
    std::unordered_map<std::string, std::any> values;

  public:
    auto define(std::string const &name, std::any const &value) {
      values[name] = value;
    }

    auto get(Token const &name) {
      if (values.contains(name.lexeme)) {
        return values[name.lexeme];
      }

      throw ReportError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    auto assign(Token const &name, std::any const &value) {
      if (values.contains(name.lexeme)) {
        values[name.lexeme] = value;
        return;
      }

      throw ReportError(name, "Undefined variable '" + name.lexeme + "'.");
    }
  };
} // namespace lox