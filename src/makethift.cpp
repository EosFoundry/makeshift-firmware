#include <makethift.hpp>

inline namespace mkshft_lisp {
std::map<std::string, Lfunc> unsafe_callables;
std::function<void(std::string)> loggingFunction;
bool logFuncSet = false;

void init() { unsafe_callables.emplace("ui", &unsafe_callUI); }
void init(std::function<void(std::string)> logFunc) {
  loggingFunction = logFunc;
  logFuncSet = true;
  init();
}

std::list<Ltoken> tokenize(std::string exp) {
  std::list<Ltoken> tokens;
  Ltoken currToken;

  loggingFunction("starting tokenization on: ");
  log("Tokenizing expression: \"");
  log(exp);
  log("\"");

  auto expCursor = exp.begin();
  auto end = exp.end();

  TokenType lastState = SPC;
  TokenType currState = SPC;

  bool numSigned = false;
  bool numDecimal = false;
  char cursorTarget = 'a';
  while (expCursor != end) {
    // set temp variable to prevent modification of exp
    cursorTarget = *expCursor;
    if (cursorTarget == CarKeys::LIST_OPEN ||
        cursorTarget == CarKeys::LIST_CLOSE) {
      currState = PAR;
    } else if (cursorTarget == CarKeys::SPACE ||
               cursorTarget == CarKeys::NEWLINE ||
               cursorTarget == CarKeys::TAB) {
      currState = SPC;
    } else if (cursorTarget >= 0x30 && cursorTarget <= 0x39) {
      currState = NUM;
    } else if (cursorTarget == '.') {
      if (numDecimal) {
        currState = SYM;
      } else {
        currState = NUM;
      }
    } else if (cursorTarget == '-') {
      if (lastState == SYM || lastState == NUM) {
        currState = SYM;
      } else {
        currState = NUM;
      }
    } else {
      currState = SYM;
    }

#if DEBUG_MKLISP > 6
    Serial.print("targ: ");
    Serial.print(cursorTarget);
    Serial.print(" | crState : ");
    Serial.print(currState);
    Serial.print(" | lsState : ");
    Serial.print(lastState);
#endif

    // if it's different, new token, no exceptions
    if (currState != lastState) {
      numDecimal = false; // reset decimal state
      if (lastState != UNDEF && lastState != SPC) {
#if DEBUG_MKLISP > 6
        Serial.print(" | pushing token: \'");
        Serial.print(currToken.value.data());
        Serial.print('\'');
#endif
        tokens.push_back(currToken);
      }
      currToken.type = currState;
      currToken.value = "";
    } else {
      switch (currState) {
      case PAR:
        tokens.push_back(currToken);
        currToken.type = currState;
        currToken.value = "";
        break;
      case SPC:
        // replaces all formatting spaces (\n \t, etc.) to space character " "
        currToken.value += " ";
        break;
      case NUM: {
        bool isSign = (cursorTarget == '+' || cursorTarget == '-');
        bool isDeci = (cursorTarget == '.');
        bool isSym = numSigned && isSign;
        isSym = numDecimal && isSign;
        isSym = numDecimal && isDeci;
        if (isSym) {
          currToken.type = SYM;
        }
        if (isDeci) {
          numDecimal = true;
        }
        break;
      }
      default:
        break;
      }
    }
#if DEBUG_MKLISP > 6
    Serial.println();
#endif
    currToken.value.push_back(cursorTarget);
    lastState = currState;
    ++expCursor;
  }

  if (currToken.value.length() > 0 && currToken.type != SPC &&
      currToken.type != UNDEF) {
    tokens.push_back(currToken);
  }

  return tokens;
}

SymExp matchParens(std::list<Ltoken> tokenList) {
  Serial.println("matching parens");
  if (tokenList.begin()->type != PAR) {
    return error(LerrType::SYNTAX_ERROR);
  }
  int depth = 0;
  int netParens = 0;
  for (auto token : tokenList) {
    switch (token.type) {
    case PAR:
      if (*token.value.begin() == CarKeys::LIST_OPEN) {
        ++depth;
        ++netParens;
      }
      if (*token.value.begin() == CarKeys::LIST_CLOSE) {
        --netParens;
      }
      break;
    default:
      break;
    }
  }
  if (netParens > 0) {
    return error(SYNTAX_ERROR, "missing close bracket/s");
  }
  if (netParens < 0) {
    return error(SYNTAX_ERROR, "missing open bracket/s");
  }

  return symInt(depth);
}

SymExp parseTokens(std::list<Ltoken> tokenList) {
  Serial.println("parser entry point");
  SymExp parenErr = matchParens(tokenList);
  if (parenErr.type != ERROR) {
    return unsafe_parse(tokenList);
  } else {
    return parenErr;
  }
}

SymExp unsafe_parse(std::list<Ltoken> tokenList) {
  if (tokenList.size() == 1) {
    auto token = tokenList.begin();
    SymExp sexp;

    switch (sexp.type) {
    case SYM:
      return symStr(token->value);
      break;
    case NUM:
      // TODO: parse for float and integer
      if (token->value.find('.') == std::string::npos) {
        return symInt(atoi(token->value.data()));
      } else {
        return symFlt(atof(token->value.data()));
      }
      break;

    case PAR:
      // auto subList = tokenList.splice()
      break;
    default:
      break;
    }
  } else {
    SymExp sexp;
  }
}

SymExp symInt(int val) {
  SymExp s;
  s.type = INTEGER;
  s.numInt = val;
  s.numFloat = (float)val;
  return s;
}

SymExp symFlt(float val) {
  SymExp s;
  s.type = FLOAT;
  s.numInt = (int)roundf(val);
  s.numFloat = val;
  return s;
}
SymExp symStr(std::string val) {
  SymExp s;
  s.type = SYMBOL;
  s.sym = val;
  return s;
}

SymExp symList(std::list<SymExp> val) {
  SymExp s;
  s.type = LIST;
  s.list = val;
  s.listItr = s.list.begin();
  return s;
}

SymExp cons(SymExp left, SymExp right) {
  SymExp s;
  s.type = LIST;
  s.list.push_back(left);
  s.list.push_back(right);
  s.listItr = s.list.begin();
  return s;
}

SymExp eval(SymExp symexp) { return symexp; }

SymExp car(SymExp symexp) {
  switch (symexp.type) {
  case LIST:
    if (symexp.list.empty()) {
      // Empty symexp initializes to NIL
      return SymExp();
    } else {
      return SymExp(*symexp.list.begin());
    }
  default:
    return symexp;
  }
}

SymExp quote(SymExp symexp) {
  switch (symexp.type) {
  case ERROR:
    symexp.type = SYMBOL;
    return symexp;
    break;
  case SYMBOL:
    return symexp;
  case LIST:
    return quoteList(symexp);
  case INTEGER:
    return quoteNumInt(symexp);
  case FLOAT:
    return quoteNumFlt(symexp);
  case NIL:
  default:
    return SymExp();
  }
}

SymExp quoteNumInt(SymExp exp) {
  SymExp ret;
  ret.type = SYMBOL;

  uint8_t base = 10;
  int len = 1;
  int num = exp.numInt;
  uint8_t buf[34];
  bool sign = (num > 0);

  // while (true) {
  //   num = num / 10;
  //   // digit = num % base;
  //   // buf[idx] = ((digit < 10) ? '0' + digit : 'A' + digit - 10);
  //   if (num == 0) {
  //     break;
  //   }
  //   ++len;
  // }
  // snprintf((char *)buf, len, "%u", num);

  uint8_t digit;
  // start the count backwards from least significant digit
  uint8_t idx = sizeof(buf) - 1;

  if (num == 0) {
    buf[sizeof(buf) - 1] = '0';
  } else {
    // uses modulo operator
    while (1) {
      digit = num % base;
      buf[idx] = ((digit < 10) ? '0' + digit : 'A' + digit - 10);
      num /= base;
      if (num == 0)
        break;
      idx--;
    }
  }
  if (sign) {
    idx--;
    buf[idx] = '-';
  }
  ret.type = SYMBOL;
  ret.sym = std::string(std::begin(buf) + idx, std::end(buf) - 1);
  return ret;
}

SymExp quoteNumFlt(SymExp flt) {
  uint8_t len = 34;
  uint8_t buf[34];
  snprintf((char *)buf, len, "%f", flt.numFloat);
}

SymExp log(std::string s) {
  if (logFuncSet) {
    loggingFunction(s);
    return SymExp();
  } else {
    return error(UNKNOWN_SYMBOL, "loggingFunction");
  }
}
SymExp logln(std::string s) {
  log(s);
  auto ret = log("\n");
  return ret;
}
SymExp error(LerrType errType) {
  SymExp err;
  err.type = ERROR;
  err.errType = errType;
  err.sym = LerrMsg[errType];
  log(err.sym);
  return err;
}

SymExp error(LerrType errType, SymExp errSym) {
  // calls error to set type and message
  SymExp err = error(errType);

  // adds the symbol to the returned error for further debugging
  if (errSym.type == LIST) {
    err.list = errSym.list;
  } else {
    err.list.push_front(errSym);
  }
  err.listItr = err.list.begin();
  return err;
}

SymExp error(LerrType errType, std::string msg) {
  Serial.print(msg.data());
  SymExp err = error(errType);
  err.sym.append(" - " + msg);
  return err;
}

SymExp error(LerrType errType, SymExp errSym, std::string msg) {
  Serial.print(msg.data());
  SymExp err = error(errType, errSym);
  err.sym.append(" - " + msg);
  return err;
}

SymExp unsafe_callUI(SymExp symexp) {
  // if (symexp.type == LIST && symexp.data.begin()->type == SYMBOL) {
  //   auto sexpItr = symexp.data.begin();
  // }
  return SymExp();
}
} // namespace mkshft_lisp