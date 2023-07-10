#pragma once

#include <any>
#include <string>
#include <utility>
#include <variant>

#include "Expr.hpp"

// helper type for the visitor #4
template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace lox {
  using LiteralVal = std::variant<std::monostate, std::string, bool, double>;
  [[nodiscard]] static auto to_string(LiteralVal const &literal)
      -> std::string {
    using namespace std::string_literals;
    return std::visit(
        overloaded{[](std::monostate const &arg) { return "nil"s; },
                   [](std::string const &arg) { return arg; },
                   [](bool const &arg) { return arg ? "true"s : "false"s; },
                   [](double const &arg) { return std::to_string(arg); }},
        literal);
  }

  enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    END_OF_FILE
  };

  struct Token {
    const TokenType type;
    const std::string lexeme;
    const LiteralVal literal;
    const int line;

    Token(TokenType type, std::string lexeme, LiteralVal literal, int line)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)),
          line(line) {}

    [[nodiscard]] auto toString() const -> std::string {
      std::string typeString;
      std::string literalString;

      switch (type) {
        case (TokenType::IDENTIFIER):
          literalString = lexeme;
          break;
        case (TokenType::STRING):
        case (TokenType::NUMBER):
        case (TokenType::TRUE):
        case (TokenType::FALSE):
        default:
          literalString = ::lox::to_string(literal);
      }

      switch (type) {
        case TokenType::LEFT_PAREN:
          typeString = "LEFT_PAREN";
          break;
        case TokenType::RIGHT_PAREN:
          typeString = "RIGHT_PAREN";
          break;
        case TokenType::LEFT_BRACE:
          typeString = "LEFT_BRACE";
          break;
        case TokenType::RIGHT_BRACE:
          typeString = "RIGHT_BRACE";
          break;
        case TokenType::COMMA:
          typeString = "COMMA";
          break;
        case TokenType::DOT:
          typeString = "DOT";
          break;
        case TokenType::MINUS:
          typeString = "MINUS";
          break;
        case TokenType::PLUS:
          typeString = "PLUS";
          break;
        case TokenType::SEMICOLON:
          typeString = "SEMICOLON";
          break;
        case TokenType::SLASH:
          typeString = "SLASH";
          break;
        case TokenType::STAR:
          typeString = "STAR";
          break;
        case TokenType::BANG:
          typeString = "BANG";
          break;
        case TokenType::BANG_EQUAL:
          typeString = "BANG_EQUAL";
          break;
        case TokenType::EQUAL:
          typeString = "EQUAL";
          break;
        case TokenType::EQUAL_EQUAL:
          typeString = "EQUAL_EQUAL";
          break;
        case TokenType::GREATER:
          typeString = "GREATER";
          break;
        case TokenType::GREATER_EQUAL:
          typeString = "GREATER_EQUAL";
          break;
        case TokenType::LESS:
          typeString = "LESS";
          break;
        case TokenType::LESS_EQUAL:
          typeString = "LESS_EQUAL";
          break;
        case TokenType::IDENTIFIER:
          typeString = "IDENTIFIER";
          break;
        case TokenType::STRING:
          typeString = "STRING";
          break;
        case TokenType::NUMBER:
          typeString = "NUMBER";
          break;
        case TokenType::AND:
          typeString = "AND";
          break;
        case TokenType::CLASS:
          typeString = "CLASS";
          break;
        case TokenType::ELSE:
          typeString = "ELSE";
          break;
        case TokenType::FALSE:
          typeString = "FALSE";
          break;
        case TokenType::FUN:
          typeString = "FUN";
          break;
        case TokenType::FOR:
          typeString = "FOR";
          break;
        case TokenType::IF:
          typeString = "IF";
          break;
        case TokenType::NIL:
          typeString = "NIL";
          break;
        case TokenType::OR:
          typeString = "OR";
          break;
        case TokenType::PRINT:
          typeString = "PRINT";
          break;
        case TokenType::RETURN:
          typeString = "RETURN";
          break;
        case TokenType::SUPER:
          typeString = "SUPER";
          break;
        case TokenType::THIS:
          typeString = "THIS";
          break;
        case TokenType::TRUE:
          typeString = "TRUE";
          break;
        case TokenType::VAR:
          typeString = "VAR";
          break;
        case TokenType::WHILE:
          typeString = "WHILE";
          break;
        case TokenType::END_OF_FILE:
          typeString = "END_OF_FILE";
          break;
      }

      return typeString + " " + lexeme + " " + literalString;
    }
  };
} // namespace lox
