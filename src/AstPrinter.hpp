#pragma once

#include <any>
#include <memory>
#include <vector>

#include "Expr.hpp"

namespace lox {
  class AstPrinter : public expr::Visitor {
  public:
    auto print(std::shared_ptr<expr::Expr> expr) -> std::string {
      return std::any_cast<std::string>(expr->accept(*this));
    }

    auto parenthesize(std::string name,
                      const std::vector<std::shared_ptr<expr::Expr>> &exprs) {
      std::string result = "(" + name;

      for (auto expr : exprs) {
        result += " ";
        result += std::any_cast<std::string>(expr->accept(*this));
      }

      result += ")";

      return result;
    }

    auto visitBinaryExpr(expr::Binary const &expr) -> std::any override {
      return parenthesize(expr.op.lexeme, {expr.left, expr.right});
    }

    auto visitGroupingExpr(expr::Grouping const &expr) -> std::any override {
      return parenthesize("group", {expr.expression});
    }

    auto visitLiteralExpr(expr::Literal const &expr) -> std::any override {
      return expr.value.has_value()
                 ? std::to_string(std::any_cast<double>(expr.value))
                 : "nil";
    }

    auto visitUnaryExpr(expr::Unary const &expr) -> std::any override {
      return parenthesize(expr.op.lexeme, {expr.right});
    }

    auto visitAssignExpr(expr::Assign const &expr) -> std::any override {
      return parenthesize(expr.name.lexeme, {expr.value});
    }

    auto visitVariableExpr(expr::Variable const &expr) -> std::any override {
      return expr.name.lexeme;
    }

    auto visitLogicalExpr(expr::Logical const &expr) -> std::any override {
      return parenthesize(expr.op.lexeme, {expr.left, expr.right});
    }

    auto visitCallExpr(expr::Call const &expr) -> std::any override {
      return parenthesize(expr.paren.lexeme, {expr.callee});
    }

    auto visitGetExpr(expr::Get const &expr) -> std::any override {
      return parenthesize(expr.name.lexeme, {expr.object});
    }

    auto visitSetExpr(expr::Set const &expr) -> std::any override {
      return parenthesize(expr.name.lexeme, {expr.object, expr.value});
    }

    auto visitThisExpr(expr::This const &expr) -> std::any override {
      return expr.keyword.lexeme;
    }

    auto visitSuperExpr(expr::Super const &expr) -> std::any override {
      return expr.keyword.lexeme;
    }
  };
} // namespace lox