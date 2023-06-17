#pragma once

#include "Expr.hpp"
#include <any>
#include <memory>

namespace lox {
  namespace stmt {
    using expr::Expr;

    /* #region Forward declarations */
    struct Print;
    struct Expression;

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
    };

    struct Print : public Stmt {
      std::shared_ptr<Expr> expression;

      Print(std::shared_ptr<Expr> expression) : expression{expression} {};

      auto accept(Visitor &visitor) -> void override {
        return visitor.visitPrintStmt(*this);
      }
    };

    struct Expression : public Stmt {
      std::shared_ptr<Expr> expression;

      Expression(std::shared_ptr<Expr> expression) : expression{expression} {};

      auto accept(Visitor &visitor) -> void override {
        return visitor.visitExpressionStmt(*this);
      }
    };
  } // namespace stmt
} // namespace lox