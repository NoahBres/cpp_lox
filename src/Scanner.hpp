#pragma once

#include "Lox.hpp"
#include "Parser.hpp"
#include "Report.hpp"
#include "Token.hpp"
#include <unordered_map>
#include <utility>

namespace lox {

enum class ScannerStatus { UNPROCESSED, SUCCESS, HAS_ERRORS };

class Scanner {
private:
  std::string source;
  std::vector<Token> tokens;

  Report<ScannerStatus> report{ScannerStatus::UNPROCESSED};

  int start = 0;
  int current = 0;
  int line = 1;

  std::unordered_map<std::string, TokenType> RESERVED_KEYWORDS = {
      {"and", lox::TokenType::AND},       {"class", lox::TokenType::CLASS},
      {"else", lox::TokenType::ELSE},     {"false", lox::TokenType::FALSE},
      {"for", lox::TokenType::FOR},       {"fun", lox::TokenType::FUN},
      {"if", lox::TokenType::IF},         {"nil", lox::TokenType::NIL},
      {"or", lox::TokenType::OR},         {"print", lox::TokenType::PRINT},
      {"return", lox::TokenType::RETURN}, {"super", lox::TokenType::SUPER},
      {"this", lox::TokenType::THIS},     {"true", lox::TokenType::TRUE},
      {"var", lox::TokenType::VAR},       {"while", lox::TokenType::WHILE},
  };

  inline auto isAtEnd() -> bool { return current >= source.size(); }

  inline auto advance() -> char { return source.at(current++); }

  inline auto addToken(lox::TokenType type) -> void {
    tokens.emplace_back(type, source.substr(start, current - start), std::any{},
                        line);
  }

  inline auto addToken(lox::TokenType type, std::any literal) -> void {
    tokens.emplace_back(type, source.substr(start, current - start), literal,
                        line);
  }

  auto match(char expected) -> bool {
    if (isAtEnd())
      return false;
    if (source.at(current) != expected)
      return false;

    current++;
    return true;
  }

  inline auto peek() -> char {
    if (isAtEnd())
      return '\0';
    return source.at(current);
  }

  inline auto peekNext() -> char {
    if (current + 1 >= source.size())
      return '\0';
    return source.at(current + 1);
  }

  auto string() -> void {
    while (peek() != '"' && !isAtEnd()) {
      if (peek() == '\n')
        line++;
      advance();
    }

    if (isAtEnd()) {
      report.addError(ReportError(line, "Unterminated string."));
      return;
    }

    // The closing "
    advance();

    // Trim the surrounding quotes
    std::string value = source.substr(start + 1, (current - start) - 1);
    addToken(lox::TokenType::STRING, value);
  }

  auto number() -> void {
    while (std::isdigit(peek()))
      advance();

    // Look for a fractional part
    if (peek() == '.' && std::isdigit(peekNext())) {
      // Consume the "."
      advance();

      while (std::isdigit(peek()))
        advance();
    }

    addToken(lox::TokenType::NUMBER,
             std::stod(source.substr(start, current - start)));
  }

  auto identifier() -> void {
    while (std::isalnum(peek()))
      advance();

    std::string text = source.substr(start, current - start);
    auto type = RESERVED_KEYWORDS.find(text);
    if (type == RESERVED_KEYWORDS.end())
      addToken(lox::TokenType::IDENTIFIER);
    else
      addToken(type->second);
  }

  auto scanToken() -> void {
    char c = advance();
    switch (c) {
    case '(':
      addToken(lox::TokenType::LEFT_PAREN);
      break;
    case ')':
      addToken(lox::TokenType::RIGHT_PAREN);
      break;
    case '{':
      addToken(lox::TokenType::LEFT_BRACE);
      break;
    case '}':
      addToken(lox::TokenType::RIGHT_BRACE);
      break;
    case ',':
      addToken(lox::TokenType::COMMA);
      break;
    case '.':
      addToken(lox::TokenType::DOT);
      break;
    case '-':
      addToken(lox::TokenType::MINUS);
      break;
    case '+':
      addToken(lox::TokenType::PLUS);
      break;
    case ';':
      addToken(lox::TokenType::SEMICOLON);
      break;
    case '*':
      addToken(lox::TokenType::STAR);
      break;
    case '!':
      addToken(match('=') ? lox::TokenType::BANG_EQUAL : lox::TokenType::BANG);
      break;
    case '=':
      addToken(match('=') ? lox::TokenType::EQUAL_EQUAL
                          : lox::TokenType::EQUAL);
      break;
    case '<':
      addToken(match('=') ? lox::TokenType::LESS_EQUAL : lox::TokenType::LESS);
      break;
    case '>':
      addToken(match('=') ? lox::TokenType::GREATER_EQUAL
                          : lox::TokenType::GREATER);
      break;
    case '/':
      if (match('/')) {
        // A comment goes until the end of the line
        while (peek() != '\n' && !isAtEnd())
          advance();
      } else {
        addToken(lox::TokenType::SLASH);
      }
      break;
    case ' ':
    case '\r':
    case '\t':
      // Ignore whitespace
      break;
    case '\n':
      line++;
      break;
    case '"':
      string();
      break;
    default:
      if (std::isdigit(c)) {
        number();
      } else if (std::isalpha(c)) {
        identifier();
      } else {
        report.addError(ReportError(line, "Unexpected character."));
      }

      break;
    }
  }

public:
  Scanner(std::string source) : source{source} {}

  auto scanTokens() {
    while (!isAtEnd()) {
      // Beginning of next lexeme
      start = current;
      scanToken();
    }

    // Append EOF token
    tokens.emplace_back(lox::TokenType::END_OF_FILE, "", std::any{}, line);

    report.status = report.errors.size() == 0 ? ScannerStatus::SUCCESS
                                              : ScannerStatus::HAS_ERRORS;
    return std::make_pair(tokens, report);
  }
};

} // namespace lox
