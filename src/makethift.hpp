#ifndef MKSHFT_LISP_H_
#define MKSHFT_LISP_H_

#include <Arduino.h>

#include <functional>
#include <list>
#include <map>
#include <string>
#include <vector>

#include <color.hpp>
#include <mkshft_core.hpp>
#include <mkshft_display.hpp>
#include <mkshft_led.hpp>
#include <mkshft_ui.hpp>

#define DEBUG_MKLISP 7
inline namespace mkshft_lisp {
enum CarKeys {
  LIST_OPEN = '[',
  LIST_CLOSE = ']',
  SPACE = ' ',
  TAB = '\t',
  NEWLINE = '\n',
  QUOTE = '\'',
  BACKQUOTE = ',',
  // LITERAL_OPEN = '"',
  // LITERAL_CLOSE = '"',
  ESC = '\\',
};

enum SexpType {
  NIL,
  INTEGER,
  SYMBOL,
  ERROR,
  LIST,
  FLOAT,
};
enum LerrType {
  NO_ERROR,
  UNKNOWN_OP,
  TYPE_ERROR,
  DIV_ZERO,
  SYNTAX_ERROR,
  UNKNOWN_SYMBOL,
};
const std::string LerrMsg[5] = {
    "Error: No error!...???", "Error: Unknown Operation",
    "Error: Wrong type given", "Error: Division by zero",
    "Error: Syntax error"};
enum TokenType {
  PAR,
  SPC,
  SYM,
  NUM,
  UNDEF,
};

struct SymExp {
  SexpType type = NIL;
  int numInt = 0;
  float numFloat = 0.0;
  std::string sym = "";
  LerrType errType = NO_ERROR;
  std::list<SymExp> list;
  std::list<SymExp>::iterator listItr = list.begin();
};

struct Ltoken {
  TokenType type;
  std::string value;
};

typedef std::function<SymExp(SymExp)> Lfunc;

extern std::map<std::string, SymExp> symbols;
extern std::map<std::string, Lfunc> unsafe_callables;
extern std::function<void(std::string)> loggingFunction;
extern bool logFuncSet;

void init();
void init(std::function<void(std::string)> logFunc);

std::list<Ltoken> tokenize(std::string);
SymExp matchParens(std::list<Ltoken>);
SymExp parseTokens(std::list<Ltoken>);

// std::list<Lval> atomize
SymExp symInt(int);
SymExp symFlt(float);
SymExp symStr(std::string);
SymExp cons(SymExp, SymExp);
SymExp eval(SymExp);
SymExp car(SymExp);

SymExp quote(SymExp);
SymExp log(std::string);
SymExp log(int);
SymExp logln(std::string);
SymExp error(LerrType);
SymExp error(LerrType, SymExp);
SymExp error(LerrType, std::string);
SymExp error(LerrType, SymExp, std::string);

// unsafe calls where bad inputs can segfault or memleak
SymExp unsafe_parse(std::list<Ltoken>);
SymExp quoteNumInt(SymExp);
SymExp quoteNumFlt(SymExp);
SymExp quoteList(SymExp);
SymExp quoteSym(SymExp);
SymExp unsafe_callUI(SymExp);
} // namespace mkshft_lisp
#endif