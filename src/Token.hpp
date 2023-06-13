#pragma once

#include <any>
#include <string>

namespace lox {

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
  const std::any literal;
  const int line;

  Token(TokenType type, const std::string &lexeme, const std::any &literal,
        int line)
      : type(type), lexeme(lexeme), literal(literal), line(line) {}

  auto toString() const -> std::string {
    std::string typeString;
    std::string literalString;

    switch (type) {
    case (TokenType::IDENTIFIER):
      literalString = lexeme;
      break;
    case (TokenType::STRING):
      literalString = std::any_cast<std::string>(literal);
      break;
    case (TokenType::NUMBER):
      literalString = std::to_string(std::any_cast<double>(literal));
      break;
    case (TokenType::TRUE):
      literalString = "true";
      break;
    case (TokenType::FALSE):
      literalString = "false";
      break;
    default:
      literalString = "nil";
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
