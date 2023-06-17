#pragma once

#include <any>
#include <memory>
#include <vector>

#include "Expr.hpp"

namespace lox {
  class AstPrinter : public lox::Visitor {
  public:
    auto print(std::shared_ptr<Expr> expr) -> std::string {
      return std::any_cast<std::string>(expr->accept(*this));
    }

    auto parenthesize(std::string name,
                      const std::vector<std::shared_ptr<Expr>> &exprs) {
      std::string result = "(" + name;

      for (auto expr : exprs) {
        result += " ";
        result += std::any_cast<std::string>(expr->accept(*this));
      }

      result += ")";

      return result;
    }

    auto visitBinaryExpr(Binary const &expr) -> std::any override {
      return parenthesize(expr.op.lexeme, {expr.left, expr.right});
    }

    auto visitGroupingExpr(Grouping const &expr) -> std::any override {
      return parenthesize("group", {expr.expression});
    }

    auto visitLiteralExpr(Literal const &expr) -> std::any override {
      return expr.value.has_value()
                 ? std::to_string(std::any_cast<double>(expr.value))
                 : "nil";
    }

    auto visitUnaryExpr(Unary const &expr) -> std::any override {
      return parenthesize(expr.op.lexeme, {expr.right});
    }

    auto visitAssignExpr(Assign const &expr) -> std::any override {
      return parenthesize(expr.name.lexeme, {expr.value});
    }

    auto visitVariableExpr(Variable const &expr) -> std::any override {
      return expr.name.lexeme;
    }

    auto visitLogicalExpr(Logical const &expr) -> std::any override {
      return parenthesize(expr.op.lexeme, {expr.left, expr.right});
    }

    auto visitCallExpr(Call const &expr) -> std::any override {
      return parenthesize(expr.paren.lexeme, {expr.callee});
    }

    auto visitGetExpr(Get const &expr) -> std::any override {
      return parenthesize(expr.name.lexeme, {expr.object});
    }

    auto visitSetExpr(Set const &expr) -> std::any override {
      return parenthesize(expr.name.lexeme, {expr.object, expr.value});
    }

    auto visitThisExpr(This const &expr) -> std::any override {
      return expr.keyword.lexeme;
    }

    auto visitSuperExpr(Super const &expr) -> std::any override {
      return expr.keyword.lexeme;
    }
  };
} // namespace lox