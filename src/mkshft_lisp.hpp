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

#define LOGLVL_MKSHFT_LISP LOGLVL_TRACE

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
  UNKNOWN_OP,
  TYPE_ERROR,
  DIV_ZERO,
  SYNTAX_ERROR,
  UNKNOWN_SYMBOL,
  MISMATCH_ARGUMENTS,
  NO_ERROR,
};

const std::string LerrMsg[LerrType::NO_ERROR + 1] = {
    "Error: Unknown Operation",
    "Error: Wrong type given",
    "Error: Division by zero",
    "Error: Syntax error",
    "Error: Unknown symbol",
    "Error: No matching function for given arguments",
    "Error: No error!...???",
};

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
  float numFlt = 0.0;
  std::string sym = "";
  std::list<SymExp> list;
  LerrType errType = NO_ERROR;
  SymExp(std::list<SymExp> l) : type(LIST), list(l){};
  SymExp(std::string s) : type(SYMBOL), sym(s){};
  SymExp(float f) : type(FLOAT), numFlt(f) { numInt = (int)roundf(f); };
  SymExp(int i) : type(INTEGER), numInt(i), numFlt((float)i){};
  SymExp() {}
};

struct Ltoken {
  TokenType type;
  std::string value;
};

typedef std::function<SymExp(SymExp)> Lfunc;

extern std::map<std::string, SymExp> symbols;
extern std::map<std::string, Lfunc> unsafe_callables;
extern std::map<std::string, Lfunc> callables;
extern std::function<void(std::string)> loggingFunction;
extern bool logFuncSet;

void init();
void init(std::function<void(std::string)> logFunc);

std::list<Ltoken> tokenize(std::string);
SymExp matchParens(std::list<Ltoken>);
SymExp wrapMultipleExpIntoList(std::list<Ltoken> tokenList);
SymExp parseTokens(std::list<Ltoken>);

// library
SymExp colorPixel(SymExp args);

// core functions
SymExp cons(SymExp);
SymExp eval(SymExp);
SymExp head(SymExp);
SymExp tail(SymExp);
SymExp map(SymExp);

// internals
SymExp nil();
SymExp toSym(SymExp);
SymExp log(std::string);
SymExp logList(std::list<SymExp>, int);
SymExp log(SymExp);
SymExp logln(std::string);
SymExp error(LerrType);
SymExp error(LerrType, SymExp);
SymExp error(LerrType, std::string);
SymExp error(LerrType, SymExp, std::string);

/**
 * checkShape() tests given SymExp against given SexpType array. If the shape
 * matches the args, it returns a nil SymExp. If the shape does not match the
 * args, it returns LerrType::TYPE_ERROR.
 *
 * @param SexpType shape - array of SexpType to match against
 * @param uint8_t len - length of shape array
 * @param SymExp args - SymExp to test
 * @return SymExp - nil if shape matches args, LerrType::TYPE_ERROR otherwise
 */
SymExp checkListShape(SexpType, uint8_t, SymExp);

/**
 * inspect() returns a string representation of the given SymExp.
 */
SymExp inspect(SymExp);

// unsafe calls where bad inputs can segfault or memleak
SymExp unsafe_parse(std::list<Ltoken>);
SymExp intToSym(SymExp);
SymExp fltToSym(SymExp);
SymExp listToSym(SymExp, int);
SymExp unsafe_callUI(SymExp);
} // namespace mkshft_lisp
#endif
