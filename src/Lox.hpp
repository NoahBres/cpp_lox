#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <optional>
#include <ranges>
#include <string>

#include "AstPrinter.hpp"
#include "Interpreter.hpp"
#include "Parser.hpp"
#include "Scanner.hpp"

namespace lox {
  class Lox {
  public:
    static auto run(const std::string &source) -> void {
      /* #region Scanning + Print tokens */
      std::cout << "[Scanning]" << '\n';
      auto scanner = lox::Scanner(source);
      auto [tokens, scannerReport] = scanner.scanTokens();

      for (const auto &token : tokens) {
        std::cout << token.toString() << std::endl;
      }

      std::cout << "" << std::endl;
      /* #endregion */

      /* #region Parsing */
      std::cout << "Parsing" << '\n';
      auto parser = lox::Parser{tokens};
      auto [statements, parsingReport] = parser.parse();

      if (parsingReport.status == ParserStatus::HAS_ERRORS) {
        parsingReport.printErrors();
        return;
      }
      /* #endregion */

      /* #region AST Printer */
      // std::cout << "AST Printer:\n";
      // std::cout << lox::AstPrinter().print(*expression) << "\n\n";
      /* #endregion */

      /* #region Interpreter */
      std::cout << "Interpreter:\n";
      auto interpreter = Interpreter{};

      // Filter and extract value from optional vector
      std::vector<decltype(statements)::value_type::value_type> filtered;
      filtered.reserve(statements.size());

      // std::ranges::views unfortunately does not like the optional<unique_ptr>
      // extraction
      for (auto &stmt : statements) {
        if (stmt.has_value()) {
          filtered.push_back(std::move(stmt.value()));
        }
      }

      interpreter.interpret(filtered);
      /* #endregion */
    }

    static auto runFile(const std::string &filePath) -> void {
      auto input = std::ifstream{filePath};
      if (!input) {
        std::cerr << "Could not open file: " << filePath << '\n';
      }

      run(std::string{std::istreambuf_iterator<char>{input}, {}});

      // TODO
      // parser error
      // if (hadError)
      // exit(65);

      // interpreter error
      // if (hadRuntimeError)
      // exit(70);
    }

    static auto runPrompt() -> void {
      auto line = std::string{};

      while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
          break;
        }

        run(line);
        // hadError = false;
      }
    }
  };

} // namespace lox