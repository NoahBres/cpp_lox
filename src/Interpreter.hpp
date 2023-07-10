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
#include <variant>
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
    std::unique_ptr<Environment> environment = std::make_unique<Environment>();

    static auto inline isTruthy(LiteralVal const &object) {
      return std::visit(
          overloaded{[](std::monostate const &arg) { return false; },
                     [](std::string const &arg) { return !arg.empty(); },
                     [](bool const &arg) { return arg; },
                     [](double const &arg) { return arg != 0; }},
          object);
    }

    static auto inline isEqual(LiteralVal const &a, LiteralVal const &b) {
      // If both null
      if (std::holds_alternative<std::monostate>(a) &&
          std::holds_alternative<std::monostate>(b)) {
        return true;
      }

      // If only a is null
      if (std::holds_alternative<std::monostate>(a)) {
        return false;
      }

      // If a and b are strings
      if (std::holds_alternative<std::string>(a) &&
          std::holds_alternative<std::string>(b)) {
        return std::get<std::string>(a) == std::get<std::string>(a);
      }

      // If a and b are doubles
      if (std::holds_alternative<double>(a) &&
          std::holds_alternative<double>(b)) {
        return std::get<double>(a) == std::get<double>(a);
      }

      // If a and b are bools
      if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b)) {
        return std::get<bool>(a) == std::get<bool>(a);
      }

      return false;
    }
    static auto validateOpIsNumberThrows(Token op, LiteralVal const &operand) {
      if (std::holds_alternative<double>(operand)) {
        return;
      }

      throw ReportError(std::move(op), "Operand must be a number");
    }

    static auto validateOpIsNumberThrows(Token op, LiteralVal const &left,
                                         LiteralVal const &right) {
      if (std::holds_alternative<double>(left) &&
          std::holds_alternative<double>(right)) {
        return;
      }

      throw ReportError(std::move(op), "Operands must be numbers.");
    }

    static auto stringify(LiteralVal const &obj) {
      // Special handling to remove decimal from 0.0 double
      if (std::holds_alternative<double>(obj)) {

        auto text = to_string(obj);
        if (text.ends_with(".0")) {
          text = text.substr(0, text.length() - 2);
        }

        return text;
      }

      return to_string(obj);
    }

    auto inline evaluate(expr::Expr &expr) {
      return std::visit(
          [this](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, expr::Literal> ||
                          std::is_same_v<T, expr::Logical> ||
                          std::is_same_v<T, expr::Grouping> ||
                          std::is_same_v<T, expr::Unary> ||
                          std::is_same_v<T, expr::Variable> ||
                          std::is_same_v<T, expr::Binary> ||
                          std::is_same_v<T, expr::Assign>) {
              return (*this)(arg);
            } else {
              return LiteralVal{};
            }
          },
          expr);
    }

    auto inline execute(stmt::Stmt const &stmt) {
      std::visit([this](auto &&arg) { return (*this)(arg); }, stmt);
    }

    auto
    executeBlock(std::vector<std::unique_ptr<stmt::Stmt>> const &statements,
                 std::unique_ptr<Environment> environment) {
      auto prev = std::move(this->environment);

      try {
        this->environment = std::move(environment);

        std::ranges::for_each(statements,
                              [this](auto const &stmt) { execute(*stmt); });
      } catch (std::exception const &e) {
        // Reset the environment on exception
        this->environment = std::move(prev);
        throw;
      }

      this->environment = std::move(prev);
    }

    /* #region Expr */
    VISIT_EXPR(expr::Literal) { return expr.value; }
    VISIT_EXPR(expr::Logical) {
      auto left = evaluate(*expr.left);

      if (expr.op.type == TokenType::OR) {
        if (isTruthy(left)) {
          return left;
        }
      } else {
        if (!isTruthy(left)) {
          return left;
        }
      }

      return evaluate(*expr.right);
    }
    VISIT_EXPR(expr::Grouping) { return evaluate(*expr.expression); }
    VISIT_EXPR(expr::Unary) {
      auto right = evaluate(*expr.right);

      switch (expr.op.type) {
        case TokenType::MINUS:
          return -std::get<double>(right);
        case TokenType::BANG:
          return !isTruthy(right);
        default:
          throw std::runtime_error{"Invalid unary operator"};
      }
    }
    VISIT_EXPR(expr::Variable) { return environment->get(expr.name); }
    VISIT_EXPR(expr::Binary) {
      auto left = evaluate(*expr.left);
      auto right = evaluate(*expr.right);

      switch (expr.op.type) {
        case TokenType::GREATER:
          validateOpIsNumberThrows(expr.op, left, right);
          return std::get<double>(left) > std::get<double>(right);

        case TokenType::GREATER_EQUAL:
          validateOpIsNumberThrows(expr.op, left, right);
          return std::get<double>(left) >= std::get<double>(right);

        case TokenType::LESS:
          validateOpIsNumberThrows(expr.op, left, right);
          return std::get<double>(left) < std::get<double>(right);

        case TokenType::LESS_EQUAL:
          validateOpIsNumberThrows(expr.op, left, right);
          return std::get<double>(left) <= std::get<double>(right);

        case TokenType::BANG_EQUAL:
          return !isEqual(left, right);

        case TokenType::EQUAL_EQUAL:
          return isEqual(left, right);

        case TokenType::MINUS:
          validateOpIsNumberThrows(expr.op, right);
          return std::get<double>(left) - std::get<double>(right);

        case TokenType::SLASH:
          validateOpIsNumberThrows(expr.op, left, right);
          return std::get<double>(left) / std::get<double>(right);

        case TokenType::STAR:
          validateOpIsNumberThrows(expr.op, left, right);
          return std::get<double>(left) * std::get<double>(right);

        case TokenType::PLUS:
          // If left and right are doubles
          if (std::holds_alternative<double>(left) &&
              std::holds_alternative<double>(right)) {
            return std::get<double>(left) + std::get<double>(right);
          }

          // If left and right are strings
          if (std::holds_alternative<std::string>(left) &&
              std::holds_alternative<std::string>(right)) {
            return std::get<std::string>(left) + std::get<std::string>(right);
          }

          throw ReportError(expr.op,
                            "Operands must be two numbers or two strings.");
        default:
          throw std::runtime_error("Invalid binary operator");
      }
    }
    VISIT_EXPR(expr::Assign) {
      auto value = evaluate(*expr.value);
      environment->assign(expr.name, value);
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
      auto val = stmt.initializer ? evaluate(*stmt.initializer) : LiteralVal{};
      environment->define(stmt.name.lexeme, val);
    }
    VISIT_STMT(stmt::Block) {
      executeBlock(stmt.statements,
                   std::make_unique<Environment>(*environment));
    }
    VISIT_STMT(stmt::If) {
      if (isTruthy(evaluate(*stmt.condition))) {
        execute(*stmt.thenBranch);
      } else if (stmt.elseBranch) {
        execute(*stmt.elseBranch);
      }
    }
    VISIT_STMT(stmt::While) {
      while (isTruthy(evaluate(stmt.condition))) {
        execute(stmt.body);
      }
    }
    /* #endregion */

  public:
    auto interpret(std::ranges::input_range auto &&statements) {
      auto report = Report<InterpreterStatus>{InterpreterStatus::UNPROCESSED};

      try {
        std::ranges::for_each(
            statements,
            [this](std::unique_ptr<stmt::Stmt> &stmt) { execute(*stmt); });
        report.status = InterpreterStatus::SUCCESS;
      } catch (ReportError &err) {
        report.addError(err);
        report.status = InterpreterStatus::HAS_ERRORS;
      }

      return report;
    }
  };
} // namespace lox