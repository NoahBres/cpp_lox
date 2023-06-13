#pragma once

#include <memory>
#include <vector>

#include "Token.hpp"

namespace lox {

// Forward declarations
struct Assign;
struct Binary;
struct Call;
struct Get;
struct Grouping;
struct Literal;
struct Logical;
struct Set;
struct Super;
struct This;
struct Unary;
struct Variable;

struct Expr {
  virtual ~Expr() = default;

  class Visitor {
  public:
    virtual void visitAssignExpr(const Assign &) = 0;
    virtual void visitBinaryExpr(const Binary &) = 0;
    virtual void visitCallExpr(const Call &) = 0;
    virtual void visitGetExpr(const Get &) = 0;
    virtual void visitGroupingExpr(const Grouping &) = 0;
    virtual void visitLiteralExpr(const Literal &) = 0;
    virtual void visitLogicalExpr(const Logical &) = 0;
    virtual void visitSetExpr(const Set &) = 0;
    virtual void visitSuperExpr(const Super &) = 0;
    virtual void visitThisExpr(const This &) = 0;
    virtual void visitUnaryExpr(const Unary &) = 0;
    virtual void visitVariableExpr(const Variable &) = 0;
  };

  virtual void accept(Visitor &) const {};
};

struct Assign : public Expr {
  const Token name;
  const std::shared_ptr<Expr> value;

  Assign(Token name, std::shared_ptr<Expr> value) : name{name}, value{value} {}

  void accept(Visitor &visitor) const override {
    visitor.visitAssignExpr(*this);
  }
};

struct Binary : public Expr {
  const std::shared_ptr<Expr> left;
  const std::shared_ptr<Expr> right;
  const Token op;

  Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
      : left{left}, op{op}, right{right} {}

  void accept(Visitor &visitor) const override {
    visitor.visitBinaryExpr(*this);
  }
};

struct Call : public Expr {
  const std::shared_ptr<Expr> callee;
  const std::vector<std::shared_ptr<Expr>> arguments;
  const Token paren;

  Call(std::shared_ptr<Expr> callee, Token paren,
       std::vector<std::shared_ptr<Expr>> arguments)
      : callee{callee}, paren{paren}, arguments{arguments} {}

  void accept(Visitor &visitor) const override { visitor.visitCallExpr(*this); }
};

struct Get : public Expr {
  const std::shared_ptr<Expr> object;
  const Token name;

  Get(std::shared_ptr<Expr> object, Token name) : object{object}, name{name} {}

  void accept(Visitor &visitor) const override { visitor.visitGetExpr(*this); }
};

struct Grouping : public Expr {
  const std::shared_ptr<Expr> expression;

  Grouping(std::shared_ptr<Expr> expression) : expression{expression} {}

  void accept(Visitor &visitor) const override {
    visitor.visitGroupingExpr(*this);
  }
};

struct Literal : public Expr {
  const std::any value;

  Literal(std::any value) : value{value} {}

  void accept(Visitor &visitor) const override {
    visitor.visitLiteralExpr(*this);
  }
};

struct Logical : public Expr {
  const std::shared_ptr<Expr> left;
  const std::shared_ptr<Expr> right;
  const Token op;

  Logical(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
      : left{left}, op{op}, right{right} {}

  void accept(Visitor &visitor) const override {
    visitor.visitLogicalExpr(*this);
  }
};

struct Set : public Expr {
  const std::shared_ptr<Expr> object;
  const std::shared_ptr<Expr> value;
  const Token name;

  Set(std::shared_ptr<Expr> object, Token name, std::shared_ptr<Expr> value)
      : object{object}, name{name}, value{value} {}

  void accept(Visitor &visitor) const override { visitor.visitSetExpr(*this); }
};

struct Super : public Expr {
  const Token keyword;
  const Token method;

  Super(Token keyword, Token method) : keyword{keyword}, method{method} {}

  void accept(Visitor &visitor) const override {
    visitor.visitSuperExpr(*this);
  }
};

struct This : public Expr {
  const Token keyword;

  This(Token keyword) : keyword{keyword} {}

  void accept(Visitor &visitor) const override { visitor.visitThisExpr(*this); }
};

struct Unary : public Expr {
  const std::shared_ptr<Expr> right;
  const Token op;

  Unary(Token op, std::shared_ptr<Expr> right) : op{op}, right{right} {}

  void accept(Visitor &visitor) const override {
    visitor.visitUnaryExpr(*this);
  }
};

struct Variable : public Expr {
  const Token name;

  Variable(Token name) : name{name} {}

  void accept(Visitor &visitor) const override {
    visitor.visitVariableExpr(*this);
  }
};

}; // namespace lox