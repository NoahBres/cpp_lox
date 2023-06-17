#pragma once

#include "Expr.hpp"
#include "Token.hpp"
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace lox {

struct RuntimeError : public std::exception {
  Token const token;
  std::string const message;

  RuntimeError(Token token, std::string message)
      : token{token}, message{message} {}

  auto what() const noexcept -> const char * override {
    return message.c_str();
  }
};

class Interpreter : public lox::Visitor {

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

  auto validateOpIsNumberThrows(Token op, std::any operand) {
    if (operand.type() == typeid(double))
      return;
    throw new RuntimeError(op, "Operand must be a number");
  }

  auto validateOpIsNumberThrows(Token op, std::any left, std::any right) {
    if (left.type() == typeid(double) && right.type() == typeid(double))
      return throw new RuntimeError(op, "Operands must be numbers.");
  }

  auto stringify(std::any obj) {
    if (!obj.has_value())
      return std::string("nil");

    if (obj.type() == typeid(double)) {
      auto text = std::any_cast<std::string>(obj);
      if (text.ends_with(".0"))
        text = text.substr(0, text.length() - 2);

      return text;
    }

    return std::any_cast<std::string>(obj);
  }

public:
  auto visitLiteralExpr(Literal const &expr) -> std::any override {
    return expr.value;
  }

  auto visitGroupingExpr(Grouping const &expr) -> std::any override {
    return evaluate(expr.expression);
  }

  auto visitUnaryExpr(Unary const &expr) -> std::any override {
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

  auto visitBinaryExpr(Binary const &expr) -> std::any override {
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

      throw new RuntimeError(expr.op,
                             "Operands must be two numbers or two strings.");
    }

    throw std::runtime_error("Invalid binary operator");
  }

  void interpret(std::shared_ptr<Expr> expression) {
    try {
      auto value = evaluate(expression);
      std::cout << stringify(value) << "\n";
    } catch (RuntimeError &err) {
      // Todo: ???
      // lox::Lox::runtimeError(err);
    }
  }
};
} // namespace lox