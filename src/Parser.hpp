#pragma once

#include <algorithm>
#include <concepts>
#include <exception>
#include <memory>
#include <optional>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

#include "Expr.hpp"
#include "Report.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include "utils.hpp"

namespace lox {
  enum class ParserStatus { UNPROCESSED, SUCCESS, HAS_ERRORS };

  class Parser {
    std::vector<Token> tokens;
    int current = 0;

    Report<ParserStatus> report;

    static auto generateParserError(Token token, std::string message) {
      return ReportError(std::move(token), std::move(message));
    }

    auto previous() { return tokens.at(current - 1); }

    auto peek() { return tokens.at(current); }

    auto isAtEnd() { return peek().type == TokenType::END_OF_FILE; }

    auto advance() {
      if (!isAtEnd()) {
        current++;
      }

      return previous();
    }

    auto check(TokenType type) {
      if (isAtEnd()) {
        return false;
      }

      return peek().type == type;
    }

    auto match(TokenType type) {
      if (check(type)) {
        advance();
        return true;
      }

      return false;
    }

    auto match(std::initializer_list<TokenType> types) {
      return std::ranges::any_of(types, [this](auto &type) {
        if (check(type)) {
          advance();
          return true;
        }

        return false;
      });
    }

    auto consume(TokenType type, std::string message) -> Token {
      if (check(type)) {
        return advance();
      }

      throw generateParserError(peek(), std::move(message));
    }

    auto synchronize() -> void {
      advance();

      while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) {
          return;
        }

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

    /* #region Expr */
    auto expression() -> std::unique_ptr<expr::Expr> { return assignment(); }

    auto assignment() -> std::unique_ptr<expr::Expr> {
      auto expr = equality();
      if (match(TokenType::EQUAL)) {
        auto equals = previous();
        auto value = assignment();

        if (auto *val = std::get_if<expr::Variable>(&(*expr))) {
          auto name = val->name;
          return make_unique_variant<expr::Expr, expr::Assign>(name, value);
        }

        report.addError(ReportError{equals, "Invalid assignment target."});
      }

      return expr;
    }

    auto equality() -> std::unique_ptr<expr::Expr> {
      auto expr = comparison();

      while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        auto op = previous();
        auto right = comparison();
        expr = make_unique_variant<expr::Expr, expr::Binary>(expr, op, right);
      }

      return expr;
    }

    auto comparison() -> std::unique_ptr<expr::Expr> {
      auto expr = term();

      while (match({TokenType::GREATER, TokenType::GREATER_EQUAL,
                    TokenType::LESS, TokenType::LESS_EQUAL})) {
        auto op = previous();
        auto right = term();
        expr = make_unique_variant<expr::Expr, expr::Binary>(expr, op, right);
      }

      return expr;
    }

    auto term() -> std::unique_ptr<expr::Expr> {
      auto expr = factor();

      while (match({TokenType::MINUS, TokenType::PLUS})) {
        auto op = previous();
        auto right = factor();
        expr = make_unique_variant<expr::Expr, expr::Binary>(expr, op, right);
      }

      return expr;
    }

    auto factor() -> std::unique_ptr<expr::Expr> {
      auto expr = unary();

      while (match({TokenType::SLASH, TokenType::STAR})) {
        auto op = previous();
        auto right = unary();
        expr = make_unique_variant<expr::Expr, expr::Binary>(expr, op, right);
      }

      return expr;
    }

    auto unary() -> std::unique_ptr<expr::Expr> {
      if (match({TokenType::BANG, TokenType::MINUS})) {
        auto op = previous();
        auto right = unary();
        return make_unique_variant<expr::Expr, expr::Unary>(op, right);
      }

      return primary();
    }

    auto primary() -> std::unique_ptr<expr::Expr> {
      if (match({TokenType::FALSE})) {
        return make_unique_variant<expr::Expr, expr::Literal>(false);
      }
      if (match({TokenType::TRUE})) {
        return make_unique_variant<expr::Expr, expr::Literal>(true);
      }
      if (match({TokenType::NIL})) {
        return make_unique_variant<expr::Expr, expr::Literal>(nullptr);
      }

      if (match({TokenType::NUMBER, TokenType::STRING})) {
        return make_unique_variant<expr::Expr, expr::Literal>(
            previous().literal);
      }

      if (match({TokenType::IDENTIFIER})) {
        return make_unique_variant<expr::Expr, expr::Variable>(previous());
      }

      if (match({TokenType::LEFT_PAREN})) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return make_unique_variant<expr::Expr, expr::Grouping>(expr);
      }

      throw generateParserError(peek(), "Expect expression.");
    }
    /* #endregion */

    /* #region Stmt */
    auto block() {
      std::vector<std::unique_ptr<stmt::Stmt>> statements;

      while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(std::move(declaration()));
      }

      consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
      return statements;
    }

    auto statement() -> std::unique_ptr<stmt::Stmt> {
      if (match(TokenType::PRINT)) {
        return printStatement();
      }

      if (match(TokenType::LEFT_BRACE)) {
        return make_unique_variant<stmt::Stmt, stmt::Block>(std::move(block()));
      }

      return expressionStatement();
    }

    auto printStatement() -> std::unique_ptr<stmt::Stmt> {
      auto value = expression();
      consume(TokenType::SEMICOLON, "Expect ';' after value.");

      return make_unique_variant<stmt::Stmt, stmt::Print>(value);
    }

    auto expressionStatement() -> std::unique_ptr<stmt::Stmt> {
      auto expr = expression();
      consume(TokenType::SEMICOLON, "Expect ';' after expression.");

      return make_unique_variant<stmt::Stmt, stmt::Expression>(expr);
    }
    /* #endregion */

    /* #region Declaration */
    auto varDeclaration() -> std::unique_ptr<stmt::Stmt> {
      auto name = consume(TokenType::IDENTIFIER, "Expect variable name.");

      auto initializer = match({TokenType::EQUAL})
                             ? expression()
                             : std::unique_ptr<expr::Expr>();
      consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
      return make_unique_variant<stmt::Stmt, stmt::Var>(name, initializer);
    }

    auto declaration() -> std::unique_ptr<stmt::Stmt> {
      try {
        if (match({TokenType::VAR})) {
          return varDeclaration();
        }

        return statement();
      } catch (ReportError error) {
        synchronize();
        return {};
      }
    }
    /* #endregion */

  public:
    Parser(std::vector<Token> tokens)
        : tokens{std::move(tokens)}, report{Report(ParserStatus::UNPROCESSED)} {
    }

    auto parse() {
      auto statements = std::vector<std::unique_ptr<stmt::Stmt>>();

      while (!isAtEnd()) {
        statements.push_back(std::move(declaration()));
      }

      report.status = ParserStatus::SUCCESS;
      return std::make_pair(std::move(statements), report);

      // try {
      //   auto expr = expression();
      //   report.status = ParserStatus::SUCCESS;

      //   return std::make_pair(std::make_optional(expr), report);
      // } catch (ReportError &err) {
      //   report.status = ParserStatus::HAS_ERRORS;
      //   report.addError(err);

      //   return std::make_pair(std::nullopt, report);
      // }
    }
  };
} // namespace lox