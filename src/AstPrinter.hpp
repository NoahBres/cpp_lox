#pragma once

#include <any>
#include <concepts>
#include <initializer_list>
#include <memory>
#include <variant>

#include "Expr.hpp"
#include "utils.hpp"

namespace lox {
  class AstPrinter {
  private:
    template <typename... Exprs>
    // Checking if all arguments are of type Expr
      requires(std::same_as<expr::Expr, std::decay_t<Exprs>> && ...)
    auto parenthesize(std::string const &name, Exprs &&...exprs) {
      std::string result = "(" + name;

      ((result += " " + std::any_cast<std::string>(visit(exprs))), ...);

      result += ")";

      return result;
    }

    VISIT_EXPR(expr::Binary) {
      return parenthesize(expr.op.lexeme, *expr.left, *expr.right);
    }

    VISIT_EXPR(expr::Grouping) {
      return parenthesize("group", *expr.expression);
    }

    VISIT_EXPR(expr::Literal) {
      return expr.value.has_value()
                 ? std::to_string(std::any_cast<double>(expr.value))
                 : "nil";
    }

    VISIT_EXPR(expr::Unary) {
      return parenthesize(expr.op.lexeme, *expr.right);
    }

    VISIT_EXPR(expr::Assign) {
      return parenthesize(expr.name.lexeme, *expr.value);
    }

    VISIT_EXPR(expr::Variable) { return expr.name.lexeme; }

    VISIT_EXPR(expr::Logical) {
      return parenthesize(expr.op.lexeme, *expr.left, *expr.right);
    }

    VISIT_EXPR(expr::Call) {
      return parenthesize(expr.paren.lexeme, *expr.callee);
    }

    VISIT_EXPR(expr::Get) {
      return parenthesize(expr.name.lexeme, *expr.object);
    }

    VISIT_EXPR(expr::Set) {
      return parenthesize(expr.name.lexeme, *expr.object, *expr.value);
    }

    VISIT_EXPR(expr::This) { return expr.keyword.lexeme; }

    VISIT_EXPR(expr::Super) { return expr.keyword.lexeme; }

    auto visit(expr::Expr &expr) -> std::any {
      return std::visit([this](auto &&arg) { return (*this)(arg); }, expr);
    }

  public:
    auto print(expr::Expr &expr) -> std::string {
      return std::any_cast<std::string>(visit(expr));
    }
  };
} // namespace lox