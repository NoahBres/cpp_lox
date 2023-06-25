#pragma once

#include "Expr.hpp"
#include "Token.hpp"
#include <any>
#include <memory>
#include <utility>

namespace lox::stmt {
  /* #region Forward declarations */
  struct Print;
  struct Expression;
  struct Var;

  struct Visitor;
  /* #endregion */

  struct Stmt {
    virtual ~Stmt() = default;
    virtual auto accept(Visitor &visitor) -> void{};
  };

  class Visitor {
  public:
    virtual auto visitPrintStmt(const Print &stmt) -> void{};
    virtual auto visitExpressionStmt(const Expression &stmt) -> void{};
    virtual auto visitVarStmt(const Var &stmt) -> void{};
  };

  struct Print : public Stmt {
    std::unique_ptr<expr::Expr> expression;

    Print(std::unique_ptr<expr::Expr> &expression)
        : expression{std::move(expression)} {};

    auto accept(Visitor &visitor) -> void override {
      return visitor.visitPrintStmt(*this);
    }
  };

  struct Expression : public Stmt {
    std::unique_ptr<expr::Expr> expression;

    Expression(std::unique_ptr<expr::Expr> &expression)
        : expression{std::move(expression)} {};

    auto accept(Visitor &visitor) -> void override {
      return visitor.visitExpressionStmt(*this);
    }
  };

  struct Var : public Stmt {
    Token name;
    std::optional<std::unique_ptr<expr::Expr>> initializer;

    Var(Token name, std::optional<std::unique_ptr<expr::Expr>> &initializer)
        : name{std::move(name)}, initializer{std::move(initializer)} {};

    auto accept(Visitor &visitor) -> void override {
      return visitor.visitVarStmt(*this);
    }
  };
} // namespace lox::stmt