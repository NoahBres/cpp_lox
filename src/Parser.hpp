#pragma once

#include "Expr.hpp"
#include "Report.hpp"
#include "Token.hpp"
#include <concepts>
#include <exception>
#include <memory>
#include <optional>
#include <vector>

namespace lox {

class Parser {
  std::vector<Token> tokens;
  int current = 0;

  class ParserError : public std::exception {
  public:
    ParserError(char const *message) : message{message} {}
    auto what() const noexcept -> const char * override { return message; }

  private:
    char const *message;
  };

  auto generateParserError(Token token, std::string message) {
    error(token, message);
    return ParserError{message.c_str()};
  }

  auto previous() -> Token { return tokens.at(current - 1); }

  auto peek() -> Token { return tokens.at(current); }

  auto isAtEnd() -> bool { return peek().type == TokenType::END_OF_FILE; }

  auto advance() -> Token {
    if (!isAtEnd()) {
      current++;
    }

    return previous();
  }

  auto check(TokenType type) -> bool {
    if (isAtEnd()) {
      return false;
    }

    return peek().type == type;
  }

  auto match(auto a, std::same_as<decltype(a)> auto... b) -> bool {
    return (check(a) && (check(b) && ... && (advance(), true)));
  }

  auto expression() -> std::shared_ptr<Expr> { return equality(); }

  auto equality() -> std::shared_ptr<Expr> {
    auto expr = comparison();

    while (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
      auto op = previous();
      auto right = comparison();
      expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
  }

  auto comparison() -> std::shared_ptr<Expr> {
    auto expr = term();

    while (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
                 TokenType::LESS_EQUAL)) {
      auto op = previous();
      auto right = term();
      expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
  }

  auto term() -> std::shared_ptr<Expr> {
    auto expr = factor();

    while (match(TokenType::MINUS, TokenType::PLUS)) {
      auto op = previous();
      auto right = factor();
      expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
  }

  auto factor() -> std::shared_ptr<Expr> {
    auto expr = unary();

    while (match(TokenType::SLASH, TokenType::STAR)) {
      auto op = previous();
      auto right = unary();
      expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
  }

  auto unary() -> std::shared_ptr<Expr> {
    if (match(TokenType::BANG, TokenType::MINUS)) {
      auto op = previous();
      auto right = unary();
      return std::make_shared<Unary>(op, right);
    }

    return primary();
  }

  auto primary() -> std::shared_ptr<Expr> {
    if (match(TokenType::FALSE))
      return std::make_shared<Literal>(false);
    if (match(TokenType::TRUE))
      return std::make_shared<Literal>(true);
    if (match(TokenType::NIL))
      return std::make_shared<Literal>(nullptr);

    if (match(TokenType::NUMBER, TokenType::STRING))
      return std::make_shared<Literal>(previous().literal);

    if (match(TokenType::LEFT_PAREN)) {
      auto expr = expression();
      consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
      return std::make_shared<Grouping>(expr);
    }
  }

  auto consume(TokenType type, std::string message) -> Token {
    if (check(type))
      return advance();

    throw generateParserError(peek(), message);
  }

  auto synchronize() -> void {
    advance();

    while (!isAtEnd()) {
      if (previous().type == TokenType::SEMICOLON)
        return;

      switch (peek().type) {
      case TokenType::CLASS:
      case TokenType::FUN:
      case TokenType::VAR:
      case TokenType::FOR:
      case TokenType::IF:
      case TokenType::WHILE:
      case TokenType::PRINT:
      case TokenType::RETURN:
        return;
      default:
        break;
      }

      advance();
    }
  }

public:
  Parser(std::vector<Token> tokens) : tokens{tokens} {}

  auto parse() -> std::optional<std::shared_ptr<Expr>> {
    try {
      return std::optional{expression()};
    } catch (ParserError &e) {
      return std::nullopt;
    }
  }
};

} // namespace lox