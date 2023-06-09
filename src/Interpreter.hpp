#pragma once

#include <any>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "Environment.hpp"
#include "Expr.hpp"
#include "Parser.hpp"
#include "Report.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include "utils.hpp"

namespace lox {
  enum class InterpreterStatus { UNPROCESSED, SUCCESS, HAS_ERRORS };

  class Interpreter {
  private:
    Environment environment{};

    static auto inline isTruthy(std::any const &object) -> bool {
      if (!object.has_value()) {
        return false;
      }

      if (object.type() == typeid(bool)) {
        return std::any_cast<bool>(object);
      }

      return true;
    }

    static auto inline isEqual(std::any const &a, std::any const &b) -> bool {
      if (!a.has_value() && !b.has_value()) {
        return true;
      }

      if (!a.has_value()) {
        return false;
      }

      if (a.type() == typeid(std::string) && b.type() == typeid(std::string)) {
        return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
      }

      if (a.type() == typeid(double) && b.type() == typeid(double)) {
        return std::any_cast<double>(a) == std::any_cast<double>(b);
      }

      if (a.type() == typeid(bool) && b.type() == typeid(bool)) {
        return std::any_cast<bool>(a) == std::any_cast<bool>(b);
      }

      return false;
    }
    static auto validateOpIsNumberThrows(Token op, const std::any &operand) {
      if (operand.type() == typeid(double)) {
        return;
      }

      throw ReportError(std::move(op), "Operand must be a number");
    }

    static auto validateOpIsNumberThrows(Token op, const std::any &left,
                                         const std::any &right) {
      if (left.type() == typeid(double) && right.type() == typeid(double)) {
        return;
      }

      throw ReportError(std::move(op), "Operands must be numbers.");
    }

    auto stringify(std::any obj) {
      if (!obj.has_value()) {
        return std::string("nil");
      }

      if (obj.type() == typeid(double)) {
        auto text = std::to_string(std::any_cast<double>(obj));
        if (text.ends_with(".0")) {
          text = text.substr(0, text.length() - 2);
        }

        return text;
      }

      if (obj.type() == typeid(bool)) {
        return std::any_cast<bool>(obj) ? std::string("true")
                                        : std::string("false");
      }

      if (obj.type() == typeid(std::optional<std::any>)) {
        auto value = std::any_cast<std::optional<std::any>>(obj);
        return value->has_value() ? stringify(value.value())
                                  : std::string("nil");
      }

      // TODO: Fix this casting
      return std::any_cast<std::string>(obj);
    }

    auto inline evaluate(Expr &expr) -> std::any {
      return std::visit(
          [this](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, expr::Literal> ||
                          std::is_same_v<T, expr::Grouping> ||
                          std::is_same_v<T, expr::Unary> ||
                          std::is_same_v<T, expr::Variable> ||
                          std::is_same_v<T, expr::Binary> ||
                          std::is_same_v<T, expr::Assign>) {
              return (*this)(arg);
            } else {
              return std::any{};
            }
          },
          expr);
    }

    auto inline execute(Stmt &stmt) {
      std::visit([this](auto &&arg) { return (*this)(arg); }, stmt);
    }

    /* #region Expr */
    VISIT_EXPR(expr::Literal) { return expr.value; }
    VISIT_EXPR(expr::Grouping) { return evaluate(*expr.expression); }
    VISIT_EXPR(expr::Unary) {
      auto right = evaluate(*expr.right);

      switch (expr.op.type) {
        case TokenType::MINUS:
          return -std::any_cast<double>(right);
        case TokenType::BANG:
          return !isTruthy(right);
        default:
          throw std::runtime_error{"Invalid unary operator"};
      }
    }
    VISIT_EXPR(expr::Variable) { return environment.get(expr.name); }
    VISIT_EXPR(expr::Binary) {
      auto left = evaluate(*expr.left);
      auto right = evaluate(*expr.right);

      switch (expr.op.type) {
        case TokenType::GREATER:
          validateOpIsNumberThrows(expr.op, left, right);
          return std::any_cast<double>(left) > std::any_cast<double>(right);

        case TokenType::GREATER_EQUAL:
          validateOpIsNumberThrows(expr.op, left, right);
          return std::any_cast<double>(left) >= std::any_cast<double>(right);

        case TokenType::LESS:
          validateOpIsNumberThrows(expr.op, left, right);
          return std::any_cast<double>(left) < std::any_cast<double>(right);

        case TokenType::LESS_EQUAL:
          validateOpIsNumberThrows(expr.op, left, right);
          return std::any_cast<double>(left) <= std::any_cast<double>(right);

        case TokenType::BANG_EQUAL:
          return !isEqual(left, right);

        case TokenType::EQUAL_EQUAL:
          return isEqual(left, right);

        case TokenType::MINUS:
          validateOpIsNumberThrows(expr.op, right);
          return std::any_cast<double>(left) - std::any_cast<double>(right);

        case TokenType::SLASH:
          validateOpIsNumberThrows(expr.op, left, right);
          return std::any_cast<double>(left) / std::any_cast<double>(right);

        case TokenType::STAR:
          validateOpIsNumberThrows(expr.op, left, right);
          return std::any_cast<double>(left) * std::any_cast<double>(right);

        case TokenType::PLUS:
          if (left.type() == typeid(double) && right.type() == typeid(double)) {
            return std::any_cast<double>(left) + std::any_cast<double>(right);
          }

          if (left.type() == typeid(std::string) &&
              right.type() == typeid(std::string)) {
            return std::any_cast<std::string>(left) +
                   std::any_cast<std::string>(right);
          }

          throw ReportError(expr.op,
                            "Operands must be two numbers or two strings.");
        default:
          throw std::runtime_error("Invalid binary operator");
      }
    }
    VISIT_EXPR(expr::Assign) {
      auto value = evaluate(*expr.value);
      environment.assign(expr.name, value);
      return value;
    }

    /* #endregion */

    /* #region Stmt */
    VISIT_STMT(stmt::Expression) { evaluate(*stmt.expression); }
    VISIT_STMT(stmt::Print) {
      auto value = evaluate(*stmt.expression);
      std::cout << stringify(value) << std::endl;
    }
    VISIT_STMT(stmt::Var) {
      auto val = stmt.initializer.has_value()
                     ? std::make_optional(evaluate(*stmt.initializer.value()))
                     : std::make_optional<std::any>();

      environment.define(stmt.name.lexeme, val);
    }
    /* #endregion */

  public:
    auto interpret(const std::vector<std::unique_ptr<Stmt>> &statements) {
      auto report = Report<InterpreterStatus>{InterpreterStatus::UNPROCESSED};

      try {
        for (auto const &statement : statements) {
          execute(*statement);
        }

        report.status = InterpreterStatus::SUCCESS;
      } catch (ReportError &err) {
        report.addError(err);
        report.status = InterpreterStatus::HAS_ERRORS;
      }

      return report;
    }
  };
} // namespace lox