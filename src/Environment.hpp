#pragma once

#include <any>
#include <string>
#include <unordered_map>

#include "Report.hpp"
#include "Token.hpp"

namespace lox {
  class Environment {
  private:
    Environment *const enclosing; // Enclosing scope is a raw pointer because
                                  // this environment doesn't own its parent.
                                  // Parent's lifetime should exceed this
    std::unordered_map<std::string, std::any> values;

  public:
    Environment() : enclosing{nullptr} {}
    Environment(Environment *enclosing) : enclosing{enclosing} {}

    auto define(std::string const &name, std::any const &value) {
      values[name] = value;
    }

    [[nodiscard]] auto get(Token const &name) const -> std::any {
      if (values.contains(name.lexeme)) {
        return values.at(name.lexeme);
      }

      if (enclosing != nullptr) {
        return enclosing->get(name);
      }

      throw ReportError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    auto assign(Token const &name, std::any const &value) {
      if (values.contains(name.lexeme)) {
        values[name.lexeme] = value;
        return;
      }

      if (enclosing != nullptr) {
        enclosing->assign(name, value);
      }

      throw ReportError(name, "Undefined variable '" + name.lexeme + "'.");
    }
  };
} // namespace lox