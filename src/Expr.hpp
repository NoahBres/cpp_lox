#pragma once

#include <any>
#include <memory>
#include <vector>

#include "Token.hpp"

namespace lox {
  namespace expr {
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
      virtual std::any accept(Visitor &visitor) = 0;
    };

    class Visitor {
    public:
      virtual std::any visitAssignExpr(const Assign &) { return std::any(); };
      virtual std::any visitBinaryExpr(const Binary &) { return std::any(); };
      virtual std::any visitCallExpr(const Call &) { return std::any(); };
      virtual std::any visitGetExpr(const Get &) { return std::any(); };
      virtual std::any visitGroupingExpr(const Grouping &) {
        return std::any();
      };
      virtual std::any visitLiteralExpr(const Literal &) { return std::any(); };
      virtual std::any visitLogicalExpr(const Logical &) { return std::any(); };
      virtual std::any visitSetExpr(const Set &) { return std::any(); };
      virtual std::any visitSuperExpr(const Super &) { return std::any(); };
      virtual std::any visitThisExpr(const This &) { return std::any(); };
      virtual std::any visitUnaryExpr(const Unary &) { return std::any(); };
      virtual std::any visitVariableExpr(const Variable &) {
        return std::any();
      };
    };

    struct Assign : public Expr {
      Token const name;
      std::shared_ptr<Expr> const value;

      Assign(Token name, std::shared_ptr<Expr> value)
          : name{name}, value{value} {}

      std::any accept(Visitor &visitor) override {
        return visitor.visitAssignExpr(*this);
      }
    };

    struct Binary : public Expr {
      std::shared_ptr<Expr> const left;
      std::shared_ptr<Expr> const right;
      Token const op;

      Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
          : left{left}, op{op}, right{right} {}

      std::any accept(Visitor &visitor) override {
        return visitor.visitBinaryExpr(*this);
      }
    };

    struct Call : public Expr {
      std::shared_ptr<Expr> const callee;
      std::vector<std::shared_ptr<Expr>> const arguments;
      Token const paren;

      Call(std::shared_ptr<Expr> callee, Token paren,
           std::vector<std::shared_ptr<Expr>> arguments)
          : callee{callee}, paren{paren}, arguments{arguments} {}

      std::any accept(Visitor &visitor) override {
        return visitor.visitCallExpr(*this);
      }
    };

    struct Get : public Expr {
      std::shared_ptr<Expr> const object;
      Token const name;

      Get(std::shared_ptr<Expr> object, Token name)
          : object{object}, name{name} {}

      std::any accept(Visitor &visitor) override {
        return visitor.visitGetExpr(*this);
      }
    };

    struct Grouping : public Expr {
      std::shared_ptr<Expr> const expression;

      Grouping(std::shared_ptr<Expr> expression) : expression{expression} {}

      std::any accept(Visitor &visitor) override {
        return visitor.visitGroupingExpr(*this);
      }
    };

    struct Literal : public Expr {
      std::any const value;

      Literal(std::any value) : value{value} {}

      std::any accept(Visitor &visitor) override {
        return visitor.visitLiteralExpr(*this);
      }
    };

    struct Logical : public Expr {
      std::shared_ptr<Expr> const left;
      std::shared_ptr<Expr> const right;
      Token const op;

      Logical(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
          : left{left}, op{op}, right{right} {}

      std::any accept(Visitor &visitor) override {
        return visitor.visitLogicalExpr(*this);
      }
    };

    struct Set : public Expr {
      std::shared_ptr<Expr> const object;
      std::shared_ptr<Expr> const value;
      Token const name;

      Set(std::shared_ptr<Expr> object, Token name, std::shared_ptr<Expr> value)
          : object{object}, name{name}, value{value} {}

      std::any accept(Visitor &visitor) override {
        return visitor.visitSetExpr(*this);
      }
    };

    struct Super : public Expr {
      Token const keyword;
      Token const method;

      Super(Token keyword, Token method) : keyword{keyword}, method{method} {}

      std::any accept(Visitor &visitor) override {
        return visitor.visitSuperExpr(*this);
      }
    };

    struct This : public Expr {
      Token const keyword;

      This(Token keyword) : keyword{keyword} {}

      std::any accept(Visitor &visitor) override {
        return visitor.visitThisExpr(*this);
      }
    };

    struct Unary : public Expr {
      std::shared_ptr<Expr> const right;
      Token const op;

      Unary(Token op, std::shared_ptr<Expr> right) : op{op}, right{right} {}

      std::any accept(Visitor &visitor) override {
        return visitor.visitUnaryExpr(*this);
      }
    };

    struct Variable : public Expr {
      Token const name;

      Variable(Token name) : name{name} {}

      std::any accept(Visitor &visitor) override {
        return visitor.visitVariableExpr(*this);
      }
    };
  } // namespace expr
} // namespace lox