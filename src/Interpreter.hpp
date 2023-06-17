#pragma once

#include <any>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "Expr.hpp"
#include "Parser.hpp"
#include "Report.hpp"
#include "Stmt.hpp"
#include "Token.hpp"

namespace lox {
  enum class InterpreterStatus { UNPROCESSED, SUCCESS, HAS_ERRORS };

  class Interpreter : public expr::Visitor, public stmt::Visitor {
  private:
    auto inline evaluate(std::shared_ptr<Expr> expr) -> std::any {
      return expr->accept(*this);
    }

    auto inline isTruthy(std::any const &object) -> bool {
      if (!object.has_value())
        return false;

      if (object.type() == typeid(bool))
        return std::any_cast<bool>(object);

      return true;
    }

    auto inline isEqual(std::any const &a, std::any const &b) -> bool {
      if (!a.has_value() && !b.has_value())
        return true;

      if (!a.has_value())
        return false;

      if (a.type() == typeid(std::string) && b.type() == typeid(std::string))
        return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);

      if (a.type() == typeid(double) && b.type() == typeid(double))
        return std::any_cast<double>(a) == std::any_cast<double>(b);

      if (a.type() == typeid(bool) && b.type() == typeid(bool))
        return std::any_cast<bool>(a) == std::any_cast<bool>(b);

      return false;
    }

    auto inline execute(std::shared_ptr<Stmt> stmt) { stmt->accept(*this); }

    auto validateOpIsNumberThrows(Token op, std::any operand) {
      if (operand.type() == typeid(double))
        return;
      throw ReportError(op, "Operand must be a number");
    }

    auto validateOpIsNumberThrows(Token op, std::any left, std::any right) {
      if (left.type() == typeid(double) && right.type() == typeid(double))
        return;
      throw ReportError(op, "Operands must be numbers.");
    }

    auto stringify(std::any obj) {
      if (!obj.has_value())
        return std::string("nil");

      if (obj.type() == typeid(double)) {
        auto text = std::to_string(std::any_cast<double>(obj));
        if (text.ends_with(".0"))
          text = text.substr(0, text.length() - 2);

        return text;
      }

      // TODO: Replace with appropriate cast
      if (obj.type() == typeid(bool))
        return std::any_cast<bool>(obj) ? std::string("true")
                                        : std::string("false");

      // TODO: Fix this casting
      return std::any_cast<std::string>(obj);
    }

  public:
    /* #region Expr */
    auto visitLiteralExpr(expr::Literal const &expr) -> std::any override {
      return expr.value;
    }

    auto visitGroupingExpr(expr::Grouping const &expr) -> std::any override {
      return evaluate(expr.expression);
    }

    auto visitUnaryExpr(expr::Unary const &expr) -> std::any override {
      auto right = evaluate(expr.right);

      switch (expr.op.type) {
        case TokenType::MINUS:
          return -std::any_cast<double>(right);
        case TokenType::BANG:
          return !isTruthy(right);
        default:
          throw std::runtime_error{"Invalid unary operator"};
      }
    }

    auto visitBinaryExpr(expr::Binary const &expr) -> std::any override {
      auto left = evaluate(expr.left);
      auto right = evaluate(expr.right);

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
          if (left.type() == typeid(double) && right.type() == typeid(double))
            return std::any_cast<double>(left) + std::any_cast<double>(right);
          if (left.type() == typeid(std::string) &&
              right.type() == typeid(std::string))
            return std::any_cast<std::string>(left) +
                   std::any_cast<std::string>(right);

          throw ReportError(expr.op,
                            "Operands must be two numbers or two strings.");
      }

      throw std::runtime_error("Invalid binary operator");
    }
    /* #endregion */

    /* #region Stmt */
    auto visitExpressionStmt(stmt::Expression const &stmt) -> void override {
      evaluate(stmt.expression);
    }

    auto visitPrintStmt(stmt::Print const &stmt) -> void override {
      auto value = evaluate(stmt.expression);
      std::cout << stringify(value);
    }
    /* #endregion */

    auto interpret(std::vector<std::shared_ptr<Stmt>> statements) {
      auto report = Report<InterpreterStatus>{InterpreterStatus::UNPROCESSED};

      try {
        for (auto &statement : statements)
          execute(statement);

        report.status = InterpreterStatus::SUCCESS;
      } catch (ReportError &err) {
        report.addError(err);
        report.status = InterpreterStatus::HAS_ERRORS;
      }

      return report;
    }
  };
} // namespace lox