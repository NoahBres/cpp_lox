#pragma once

#include <any>
#include <memory>
#include <utility>
#include <variant>

#include "Expr.hpp"
#include "Token.hpp"

namespace lox::stmt {
  /* #region Forward declarations */
  struct Print;
  struct Expression;
  struct Var;

  using Stmt = std::variant<Print, Expression, Var>;
  /* #endregion */

  struct Print {
    std::unique_ptr<expr::Expr> expression;

    Print(std::unique_ptr<expr::Expr> &expression)
        : expression{std::move(expression)} {};
  };

  struct Expression {
    std::unique_ptr<expr::Expr> expression;

    Expression(std::unique_ptr<expr::Expr> &expression)
        : expression{std::move(expression)} {};
  };

  struct Var {
    Token name;
    std::optional<std::unique_ptr<expr::Expr>> initializer;

    Var(Token name, std::optional<std::unique_ptr<expr::Expr>> &initializer)
        : name{std::move(name)}, initializer{std::move(initializer)} {};
  };
} // namespace lox::stmt