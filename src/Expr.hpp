#pragma once

#include <any>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

#include "Token.hpp"

namespace lox::expr {
  /* #region Forward declarations */
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

  using Expr = std::variant<Assign, Binary, Call, Get, Grouping, Literal,
                            Logical, Set, Super, This, Unary, Variable>;
  /* #endregion */

  struct Assign {
    Token const name;
    std::unique_ptr<Expr> const value;

    Assign(Token name, std::unique_ptr<Expr> &value)
        : name{std::move(name)}, value{std::move(value)} {}
  };

  struct Binary {
    std::unique_ptr<Expr> const left, right;
    Token const op;

    Binary(std::unique_ptr<Expr> &left, Token op, std::unique_ptr<Expr> &right)
        : left{std::move(left)}, op{std::move(op)}, right{std::move(right)} {}
  };

  struct Call {
    std::unique_ptr<Expr> const callee;
    std::vector<std::unique_ptr<Expr>> const arguments;
    Token const paren;

    Call(std::unique_ptr<Expr> &callee, Token paren,
         std::vector<std::unique_ptr<Expr>> arguments)
        : callee{std::move(callee)}, paren{std::move(paren)},
          arguments{std::move(arguments)} {}
  };

  struct Get {
    std::unique_ptr<Expr> const object;
    Token const name;

    Get(std::unique_ptr<Expr> &object, Token name)
        : object{std::move(object)}, name{std::move(name)} {}
  };

  struct Grouping {
    std::unique_ptr<Expr> const expression;

    Grouping(std::unique_ptr<Expr> &expression)
        : expression{std::move(expression)} {}
  };

  struct Literal {
    LiteralVal const value;

    Literal(LiteralVal value) : value{std::move(value)} {}
  };

  struct Logical {
    std::unique_ptr<Expr> const left, right;
    Token const op;

    Logical(std::unique_ptr<Expr> &left, Token op, std::unique_ptr<Expr> &right)
        : left{std::move(left)}, op{std::move(op)}, right{std::move(right)} {}
  };

  struct Set {
    std::unique_ptr<Expr> const object, value;
    Token const name;

    Set(std::unique_ptr<Expr> &object, Token name, std::unique_ptr<Expr> &value)
        : object{std::move(object)}, name{std::move(name)},
          value{std::move(value)} {}
  };

  struct Super {
    Token const keyword, method;

    Super(Token keyword, Token method)
        : keyword{std::move(keyword)}, method{std::move(method)} {}
  };

  struct This {
    Token const keyword;

    This(Token keyword) : keyword{std::move(keyword)} {}
  };

  struct Unary {
    std::unique_ptr<Expr> const right;
    Token const op;

    Unary(Token op, std::unique_ptr<Expr> &right)
        : op{std::move(op)}, right{std::move(right)} {}
  };

  struct Variable {
    Token const name;

    Variable(Token name) : name{std::move(name)} {}
  };
} // namespace lox::expr