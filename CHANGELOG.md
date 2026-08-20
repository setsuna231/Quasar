# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
- Nothing yet.

## [v0.20.0] - 2026-08-19
### Added
- User-defined functions (`func` keyword, parameters, return types, recursion).
- String equality using `==` and `!=`.
- Full expression type inference for all AST node types.

## [v0.19.0] - 2026-08-19
### Added
- `fallthrough` statement for explicit match fallthrough.

## [v0.18.3] - 2026-08-17
### Fixed
- Scoping format-spec bug (print with local strings).
- Numeric `to_int()` and `to_float()` conversions.

## [v0.18.2] - 2026-07-31
### Added
- String concatenation (`+`) and repetition (`*`) for strings.

## [v0.18.1] - 2026-07-20
### Added
- `return` statement for user-defined functions.

## [v0.18.0] - 2026-07-19
### Added
- Type conversion functions: `to_int()`, `to_float()`, `to_string()`, `to_char()`, `to_bool()`.

## [v0.17.0] - 2026-07-19
### Added
- `input()` for user input.
- Expression statements.

## [v0.16.1] - 2026-07-19
### Added
- Multi-let declarations.
- Scoping fix.
- Type-aware print.

## [v0.16.0] - 2026-07-18
### Added
- `match` statement with `case`/`default`.

## [v0.15.1] - 2026-07-18
### Added
- `break` and `continue` statements.

## [v0.15.0] - 2026-07-18
### Added
- `for` loop with compound assignment.

## [v0.14.5] - 2026-07-17
### Added
- Nested scoping.

## [v0.14.4] - 2026-07-17
### Added
- Compound assignment operators (`+=`, `-=`, `*=`, `/=`, `%=`, `**=`, `//=`).

## [v0.14.3] - 2026-07-17
### Fixed
- Type checking: integers can be assigned to float, not vice versa.

## [v0.14.2] - 2026-07-17
### Added
- Pre/post increment and decrement operators (`++`, `--`).

## [v0.14.1] - 2026-07-16
### Added
- Comments (`/* ... */`).

## [v0.14.0] - 2026-07-16
### Added
- Loops: `while` and `repeat...until`.

## [v0.13.0] - 2026-07-16
### Added
- Conditional statements: `if` / `elif` / `else` (blocks required).

## [v0.12.1] - 2026-07-16
### Added
- Unary minus (`-`) and unary plus (`+`).

## [v0.12.0] - 2026-07-15
### Added
- Logical operators (`&&`, `||`, `!`).

## [v0.11.2] - 2026-07-15
### Added
- Block statements using `{ ... }`.

## [v0.11.1] - 2026-07-15
### Added
- Real booleans with `stdbool.h`, `true`/`false` output, and `bool` type in errors.

## [v0.11.0] - 2026-07-14
### Added
- Arithmetic and relational operators with full type‑checking and error handling.

## [v0.10.0] - 2026-07-14
### Added
- Arithmetic operators with full type‑checking and error handling.

## [v0.9.2] - 2026-07-13
### Fixed
- Strict error handling: all error types caught, no output on failure.

## [v0.9.1] - 2026-07-13
### Fixed
- Stray identifier bug by opening source in binary mode.

## [v0.9.0] - 2026-07-13
### Added
- Variable reassignment.

## [v0.8.0] - 2026-07-12
### Added
- Variables with `let`, symbol table, and type-aware print.

## [v0.7.1] - 2026-07-12
### Added
- Multi-argument `print`.

## [v0.7.0] - 2026-07-12
### Added
- Boolean literals `true` and `false`.

## [v0.6.0] - 2026-07-12
### Added
- Character literals with full escape support.

## [v0.5.2] - 2026-07-12
### Fixed
- Memory leaks, path separators, and error recovery.

## [v0.5.1] - 2026-07-12
### Improved
- Error message for malformed numbers.

## [v0.5.0] - 2026-07-12
### Added
- Robust error recovery and precise, human‑readable error messages.

## [v0.4.0] - 2026-07-12
### Added
- Float literals with `%g` format specifier.

## [v0.3.0] - 2026-07-11
### Added
- Multi-statement support and strings with escape sequences.

## [v0.2.0-cli-build] - 2026-07-11
### Added
- Working CLI with tidy build structure.

## [v0.0.1-lexer-parser] - 2026-07-10
### Added
- Working lexer and parser for `print(integer);`.