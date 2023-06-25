#pragma once

#include <any>
#include <memory>
#include <utility>
#include <vector>

#include "Token.hpp"

namespace lox::expr {
  /* #region Forward declarations */
  class Assign;
  class Binary;
  class Call;
  class Get;
  class Grouping;
  class Literal;
  class Logical;
  class Set;
  class Super;
  class This;
  class Unary;
  class Variable;

  class Visitor;
  /* #endregion */

  struct Expr {
    virtual ~Expr() = default;
    virtual auto accept(Visitor &visitor) -> std::any = 0;
  };

  class Visitor {
  public:
    virtual auto visitAssignExpr(Assign const &assignment) -> std::any {
      return {};
    };
    virtual auto visitBinaryExpr(Binary const &expr) -> std::any { return {}; };
    virtual auto visitCallExpr(Call const &expr) -> std::any { return {}; };
    virtual auto visitGetExpr(Get const &expr) -> std::any { return {}; };
    virtual auto visitGroupingExpr(Grouping const &expr) -> std::any {
      return {};
    };
    virtual auto visitLiteralExpr(Literal const &expr) -> std::any {
      return {};
    };
    virtual auto visitLogicalExpr(Logical const &expr) -> std::any {
      return {};
    };
    virtual auto visitSetExpr(Set const &expr) -> std::any { return {}; };
    virtual auto visitSuperExpr(Super const &expr) -> std::any { return {}; };
    virtual auto visitThisExpr(This const &expr) -> std::any { return {}; };
    virtual auto visitUnaryExpr(Unary const &expr) -> std::any { return {}; };
    virtual auto visitVariableExpr(Variable const &expr) -> std::any {
      return {};
    };
  };
  struct Assign : public Expr {
    Token const name;
    std::unique_ptr<Expr> const value;

    Assign(Token name, std::unique_ptr<Expr> &value)
        : name{std::move(name)}, value{std::move(value)} {}

    auto accept(Visitor &visitor) -> std::any override {
      return visitor.visitAssignExpr(*this);
    }
  };

  struct Binary : public Expr {
    std::unique_ptr<Expr> const left, right;
    Token const op;

    Binary(std::unique_ptr<Expr> &left, Token op, std::unique_ptr<Expr> &right)
        : left{std::move(left)}, op{std::move(op)}, right{std::move(right)} {}

    auto accept(Visitor &visitor) -> std::any override {
      return visitor.visitBinaryExpr(*this);
    }
  };

  struct Call : public Expr {
    std::unique_ptr<Expr> const callee;
    std::vector<std::unique_ptr<Expr>> const arguments;
    Token const paren;

    Call(std::unique_ptr<Expr> &callee, Token paren,
         std::vector<std::unique_ptr<Expr>> arguments)
        : callee{std::move(callee)}, paren{std::move(paren)},
          arguments{std::move(arguments)} {}

    auto accept(Visitor &visitor) -> std::any override {
      return visitor.visitCallExpr(*this);
    }
  };

  struct Get : public Expr {
    std::unique_ptr<Expr> const object;
    Token const name;

    Get(std::unique_ptr<Expr> &object, Token name)
        : object{std::move(object)}, name{std::move(name)} {}

    auto accept(Visitor &visitor) -> std::any override {
      return visitor.visitGetExpr(*this);
    }
  };

  struct Grouping : public Expr {
    std::unique_ptr<Expr> const expression;

    Grouping(std::unique_ptr<Expr> &expression)
        : expression{std::move(expression)} {}

    auto accept(Visitor &visitor) -> std::any override {
      return visitor.visitGroupingExpr(*this);
    }
  };

  struct Literal : public Expr {
    std::any const value;

    Literal(std::any value) : value{std::move(value)} {}

    auto accept(Visitor &visitor) -> std::any override {
      return visitor.visitLiteralExpr(*this);
    }
  };

  struct Logical : public Expr {
    std::unique_ptr<Expr> const left;
    std::unique_ptr<Expr> const right;
    Token const op;

    Logical(std::unique_ptr<Expr> &left, Token op, std::unique_ptr<Expr> &right)
        : left{std::move(left)}, op{std::move(op)}, right{std::move(right)} {}

    auto accept(Visitor &visitor) -> std::any override {
      return visitor.visitLogicalExpr(*this);
    }
  };

  struct Set : public Expr {
    std::unique_ptr<Expr> const object, value;
    Token const name;

    Set(std::unique_ptr<Expr> &object, Token name, std::unique_ptr<Expr> &value)
        : object{std::move(object)}, name{std::move(name)},
          value{std::move(value)} {}

    auto accept(Visitor &visitor) -> std::any override {
      return visitor.visitSetExpr(*this);
    }
  };

  struct Super : public Expr {
    Token const keyword, method;

    Super(Token keyword, Token method)
        : keyword{std::move(keyword)}, method{std::move(method)} {}

    auto accept(Visitor &visitor) -> std::any override {
      return visitor.visitSuperExpr(*this);
    }
  };

  struct This : public Expr {
    Token const keyword;

    This(Token keyword) : keyword{std::move(keyword)} {}

    auto accept(Visitor &visitor) -> std::any override {
      return visitor.visitThisExpr(*this);
    }
  };

  struct Unary : public Expr {
    std::unique_ptr<Expr> const right;
    Token const op;

    Unary(Token op, std::unique_ptr<Expr> &right)
        : op{std::move(op)}, right{std::move(right)} {}

    auto accept(Visitor &visitor) -> std::any override {
      return visitor.visitUnaryExpr(*this);
    }
  };

  struct Variable : public Expr {
    Token const name;

    Variable(Token name) : name{std::move(name)} {}

    auto accept(Visitor &visitor) -> std::any override {
      return visitor.visitVariableExpr(*this);
    }
  };
} // namespace lox::expr