#pragma once

#include <any>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

#include "Expr.hpp"
#include "Token.hpp"

namespace lox::stmt {
  /* #region Forward declarations */
  struct Print;
  struct Expression;
  struct Var;
  struct Block;
  struct If;

  using Stmt = std::variant<Print, Expression, Var, Block, If>;
  /* #endregion */

  struct Print {
    std::unique_ptr<expr::Expr> const expression;

    Print(std::unique_ptr<expr::Expr> &expression)
        : expression{std::move(expression)} {};
  };

  struct Expression {
    std::unique_ptr<expr::Expr> const expression;

    Expression(std::unique_ptr<expr::Expr> &expression)
        : expression{std::move(expression)} {};
  };

  struct Var {
    Token const name;
    std::unique_ptr<expr::Expr> const initializer;

    Var(Token &name, std::unique_ptr<expr::Expr> &initializer)
        : name{std::move(name)}, initializer{std::move(initializer)} {};
  };

  struct Block {
    std::vector<std::unique_ptr<Stmt>> const statements;

    Block(std::vector<std::unique_ptr<Stmt>> statements)
        : statements{std::move(statements)} {}
  };

  struct If {
    std::unique_ptr<expr::Expr> const condition;
    std::unique_ptr<Stmt> const thenBranch, elseBranch;

    If(std::unique_ptr<expr::Expr> &condition,
       std::unique_ptr<Stmt> &thenBranch, std::unique_ptr<Stmt> &elseBranch)
        : condition{std::move(condition)}, thenBranch{std::move(thenBranch)},
          elseBranch{std::move(elseBranch)} {}
  };

} // namespace lox::stmt