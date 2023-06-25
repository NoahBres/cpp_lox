#pragma once

#include <concepts>
#include <exception>
#include <memory>
#include <optional>
#include <tuple>
#include <utility>
#include <vector>

#include "Expr.hpp"
#include "Report.hpp"
#include "Stmt.hpp"
#include "Token.hpp"

namespace lox {
  using expr::Expr, stmt::Stmt;

  enum class ParserStatus { UNPROCESSED, SUCCESS, HAS_ERRORS };

  class Parser {
    std::vector<Token> tokens;
    int current = 0;

    Report<ParserStatus> report;

    auto generateParserError(Token token, std::string message) {
      return ReportError(token, message);
    }

    auto previous() { return tokens.at(current - 1); }

    auto peek() { return tokens.at(current); }

    auto isAtEnd() { return peek().type == TokenType::END_OF_FILE; }

    auto advance() {
      if (!isAtEnd())
        current++;

      return previous();
    }

    auto check(TokenType type) {
      if (isAtEnd())
        return false;

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
      for (TokenType type : types) {
        if (check(type)) {
          advance();
          return true;
        }
      }

      return false;
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

    /* #region Expr */
    auto expression() -> std::shared_ptr<Expr> { return assignment(); }

    auto assignment() -> std::shared_ptr<Expr> {
      auto expr = equality();
      if (match(TokenType::EQUAL)) {
        auto equals = previous();
        auto value = assignment();

        if (auto cast = dynamic_cast<expr::Variable *>(expr.get());
            cast != nullptr) {
          auto name = cast->name;
          return std::make_shared<expr::Assign>(name, value);
        }

        report.addError(ReportError{equals, "Invalid assignment target."});
      }

      return expr;
    }

    auto equality() -> std::shared_ptr<Expr> {
      auto expr = comparison();

      while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        auto op = previous();
        auto right = comparison();
        expr = std::make_shared<expr::Binary>(expr, op, right);
      }

      return expr;
    }

    auto comparison() -> std::shared_ptr<Expr> {
      auto expr = term();

      while (match({TokenType::GREATER, TokenType::GREATER_EQUAL,
                    TokenType::LESS, TokenType::LESS_EQUAL})) {
        auto op = previous();
        auto right = term();
        expr = std::make_shared<expr::Binary>(expr, op, right);
      }

      return expr;
    }

    auto term() -> std::shared_ptr<Expr> {
      auto expr = factor();

      while (match({TokenType::MINUS, TokenType::PLUS})) {
        auto op = previous();
        auto right = factor();
        expr = std::make_shared<expr::Binary>(expr, op, right);
      }

      return expr;
    }

    auto factor() -> std::shared_ptr<Expr> {
      auto expr = unary();

      while (match({TokenType::SLASH, TokenType::STAR})) {
        auto op = previous();
        auto right = unary();
        expr = std::make_shared<expr::Binary>(expr, op, right);
      }

      return expr;
    }

    auto unary() -> std::shared_ptr<Expr> {
      if (match({TokenType::BANG, TokenType::MINUS})) {
        auto op = previous();
        auto right = unary();
        return std::make_shared<expr::Unary>(op, right);
      }

      return primary();
    }

    auto primary() -> std::shared_ptr<Expr> {
      if (match({TokenType::FALSE}))
        return std::make_shared<expr::Literal>(false);
      if (match({TokenType::TRUE}))
        return std::make_shared<expr::Literal>(true);
      if (match({TokenType::NIL}))
        return std::make_shared<expr::Literal>(nullptr);

      if (match({TokenType::NUMBER, TokenType::STRING}))
        return std::make_shared<expr::Literal>(previous().literal);

      if (match({TokenType::IDENTIFIER}))
        return std::make_shared<expr::Variable>(previous());

      if (match({TokenType::LEFT_PAREN})) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<expr::Grouping>(expr);
      }

      throw generateParserError(peek(), "Expect expression.");
    }
    /* #endregion */

    /* #region Stmt */
    auto statement() -> std::shared_ptr<Stmt> {
      return match({TokenType::PRINT}) ? printStatement()
                                       : expressionStatement();
    }

    auto printStatement() -> std::shared_ptr<Stmt> {
      auto value = expression();
      consume(TokenType::SEMICOLON, "Expect ';' after value.");

      return std::make_shared<stmt::Print>(value);
    }

    auto expressionStatement() -> std::shared_ptr<Stmt> {
      auto expr = expression();
      consume(TokenType::SEMICOLON, "Expect ';' after expression.");

      return std::make_shared<stmt::Expression>(expr);
    }
    /* #endregion */

    /* #region Declaration */
    auto varDeclaration() -> std::shared_ptr<Stmt> {
      auto name = consume(TokenType::IDENTIFIER, "Expect variable name.");

      auto initializer = match({TokenType::EQUAL})
                             ? std::make_optional(expression())
                             : std::nullopt;
      consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
      return std::make_shared<stmt::Var>(name, initializer);
    }

    auto declaration() -> std::optional<std::shared_ptr<Stmt>> {
      try {
        if (match({TokenType::VAR}))
          return std::make_optional(varDeclaration());

        return std::make_optional(statement());
      } catch (ReportError error) {
        synchronize();
        return std::nullopt;
      }
    }
    /* #endregion */

  public:
    Parser(std::vector<Token> tokens)
        : tokens{tokens}, report{Report(ParserStatus::UNPROCESSED)} {}

    // auto parse() -> std::pair<std::optional<std::shared_ptr<Expr>>,
    //                           Report<ParserStatus>> {
    auto parse() {
      auto statements = std::vector<std::optional<std::shared_ptr<Stmt>>>();

      while (!isAtEnd()) {
        statements.push_back(declaration());
      }

      report.status = ParserStatus::SUCCESS;
      return std::make_pair(statements, report);

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