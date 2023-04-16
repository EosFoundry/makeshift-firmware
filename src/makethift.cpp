#include <makethift.hpp>

inline namespace mkshft_lisp {

std::map<std::string, Lfunc> unsafe_callables;
std::function<void(std::string)> loggingFunction;
bool logFuncSet = false;

void init() { unsafe_callables.emplace("colorPixel", &colorPixel); }
void init(std::function<void(std::string)> logFunc) {
  loggingFunction = logFunc;
  logFuncSet = true;
  init();
}

std::list<Ltoken> tokenize(std::string exp) {
  std::list<Ltoken> tokens;
  Ltoken currToken;
#if DEBUG_MKLISP > 6
  log("Tokenizing expression: \"");
  log(exp);
  log("\"");
#endif

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
  // Serial.println("matching parens");
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

  return SymExp(depth);
}

SymExp parseTokens(std::list<Ltoken> tokenList) {
  // logln("parser entry point");
  SymExp parenErr = matchParens(tokenList);
  if (parenErr.type != ERROR) {
    return unsafe_parse(tokenList);
  } else {
    return parenErr;
  }
}

SymExp unsafe_parse(std::list<Ltoken> tokenList) {
  SymExp ret;
  // logln("unsafe_parse entry");
  if (tokenList.front().value.at(0) == LIST_OPEN &&
      tokenList.back().value.at(0) == LIST_CLOSE) {
    // log(tokenList.front().value);
    // log(tokenList.back().value);
    tokenList.pop_front();
    tokenList.pop_back();
  }
  // log(SymExp((int)tokenList.size()));
  if (tokenList.size() == 1) { // the default case
    auto token = tokenList.front();
    // log("token type:: ");

    switch (token.type) {
    case SYM: {
      // log("SYM");
      ret = SymExp(tokenList.front().value);
      break;
    }

    case NUM: {
      // log("NUM");
      if (token.value.find('.') == std::string::npos) {
        ret = SymExp(atoi(token.value.data()));
      } else {
        ret = SymExp((float)atof(token.value.data()));
      }
      break;
    }

    default: {
      return error(TYPE_ERROR);
      break;
    }
    }
  } else { // the recursive case
    SymExp tmp;
    tmp.type = LIST;
    ret.type = LIST;

    auto token = tokenList.begin();
    int numTokens = tokenList.size();
    int tokensConsumed = 0;
    while (tokensConsumed < numTokens) {
      // log(token->value);
      delay(10);
      switch (token->type) {
      case SYM: {
        tmp = SymExp(token->value);
        break;
      }

      case NUM: {
        if (token->value.find('.') == std::string::npos) {
          tmp = SymExp(atoi(token->value.data()));
        } else {
          tmp = SymExp((float)atof(token->value.data()));
        }
        break;
      }

      case PAR: {
        std::list<Ltoken> subList;
        int depth = 0;
        // This loop consumes tokens until it matches the first open square
        // bracket to its corresponding closing bracket, digests the contents
        // and returns a list SymExp. The depth counter starts at 1, increments
        // with an open '[', decrements with a closed ']'. When depth reachees
        // 0, the loop has found the last close bracket and the result sublist
        // is sent to unsafe_parse() recursively.
        do {
          // log(token->value);
          if (token->type == PAR) { // tracks depth
            if (token->value.data()[0] == LIST_OPEN) {
              ++depth;
            } else {
              --depth;
            }
          }
          subList.push_back(*token);
          ++tokensConsumed;
          --numTokens;
          ++token;
        } while (depth != 0);

        auto parseResult = unsafe_parse(subList);
        // force overwrite to list in case of single element lists
        // the longer version: the default return for non-list atomic
        // expressions is that element
        if (parseResult.type != LIST) {
          std::list<SymExp> tempList;
          tempList.push_back(parseResult);
          tmp = SymExp(tempList);
        } else {
          tmp = parseResult;
        }
        break;
      }

      default: {
        tmp = SymExp();
        break;
      }
      }

      ret.list.push_back(tmp);
      ++tokensConsumed;
      ++token;
    }
  }
  return ret;
}

SymExp cons(SymExp left, SymExp right) {
  SymExp s = SymExp();
  s.type = LIST;
  s.list.push_back(left);
  s.list.push_back(right);
  return s;
}

SymExp eval(SymExp symexp) { return symexp; }

SymExp head(SymExp symexp) {
  switch (symexp.type) {
  case LIST:
    if (symexp.list.empty()) {
      // Empty symexp initializes to NIL
      return SymExp();
    } else {
      SymExp ret = SymExp(*symexp.list.begin());
    }
  default:
    return symexp;
  }
}

SymExp tail(SymExp symexp) {
  switch (symexp.type) {
  case LIST:
    if (symexp.list.empty()) {
      // Empty symexp initializes to NIL
      return SymExp();
    } else {
      SymExp ret = SymExp(symexp);
      ret.list.pop_front();
      return ret;
    }
  default:
    return symexp;
  }
}

SymExp map(SymExp symexp) {
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

SymExp nil() { return SymExp(); }

SymExp toSym(SymExp symexp) {
  switch (symexp.type) {
  case ERROR:
    symexp.type = SYMBOL;
    return symexp;
    break;
  case SYMBOL:
    return symexp;
  case LIST:
    return listToSym(symexp, 0);
  case INTEGER:
    return intToSym(symexp);
  case FLOAT:
    return fltToSym(symexp);
  case NIL:
  default:
    return SymExp();
  }
}

SymExp intToSym(SymExp exp) {
  SymExp ret;

  uint8_t base = 10;
  // int len = 1;
  int num = exp.numInt;
  uint8_t buf[34];
  bool sign = (num < 0);

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
  ret.sym = std::string(std::begin(buf) + idx, std::end(buf));
  return ret;
}

SymExp fltToSym(SymExp flt) {
  uint8_t len = 34;
  uint8_t buf[34];

  for (int i = 0; i < len; i++) {
    buf[i] = 0;
  }

  SymExp ret;
  int strLen = 0;
  strLen = snprintf((char *)buf, len, "%f", flt.numFlt);
  if (strLen > len) {
    strLen = len;
  }
  ret.type = SYMBOL;
  ret.sym = std::string(std::begin(buf), std::begin(buf) + strLen);
  log(ret.sym);
  return ret;
}

SymExp listToSym(SymExp lst, int depth) {
  SymExp ret = SymExp();
  ret.type = SYMBOL;
  ret.sym.append("[ ");
  for (auto sexp : lst.list) {
    if (sexp.type == LIST) {
      if (depth >= 5) {
        ret.sym.append("[...] ");
      } else {
        auto tmp = listToSym(sexp, depth + 1);
        ret.sym.append(tmp.sym);
        ret.sym.append(" ");
      }
    } else {
      ret.sym.append(toSym(sexp).sym);
      ret.sym.append(" ");
    }
  }
  ret.sym.append("]");
  return ret;
}

SymExp log(std::string s) {
  if (logFuncSet) {
    loggingFunction(s);
    return SymExp();
  } else {
    return error(UNKNOWN_SYMBOL, "loggingFunction");
  }
}

SymExp logList(std::list<SymExp> lst, int depth) {
  Serial.print("logging list, depth ");
  Serial.print(depth);
  if (depth < 5) {
    depth++;
    for (auto sexp : lst) {
      if (sexp.type == LIST) {
        return logList(sexp.list, depth);
      } else {
        return log(sexp);
      }
    }
  }
  return SymExp();
}

SymExp log(SymExp sexp) {
  switch (sexp.type) {
  case INTEGER: {
    return log(intToSym(sexp).sym);
    break;
  }
  case FLOAT: {
    return log(fltToSym(sexp).sym);
    break;
  }
  case SYMBOL: {
    return log(sexp.sym);
    break;
  }
  case ERROR: {
    return log(sexp.sym);
    break;
  }
  case LIST: {
    return log(listToSym(sexp, 0));
    break;
  }
  default: {
    return log("nil");
    break;
  }
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

SymExp checkShape(SexpType *shape, uint8_t shapeSz, SymExp args) {
  if (args.type != LIST) {
    return error(LerrType::TYPE_ERROR);
  } else {
    uint8_t shapeIdx = 0;
    for (auto sexp : args.list) {
      if (sexp.type != *(shape + shapeIdx)) {
        return error(LerrType::TYPE_ERROR);
      }
      ++shapeIdx;
    }
  }
  // return no error
  return SymExp();
}

SymExp inspect(SymExp s) {
  std::string loggable = "[ [ type";
  switch (s.type) {
  case INTEGER: {
    loggable.append("INTEGER ]");
  }
  case FLOAT: {
    loggable.append("FLOAT ]");
  }
  case SYMBOL: {
    loggable.append("SYMBOL ]");
  }
  case LIST: {
    loggable.append("LIST ]");
  }
  case ERROR: {
    loggable.append("ERROR ]");
  }
  default: {
    loggable.append("UNKNOWN ]");
  }
  }
  loggable.append(" [");
  auto stringified = toSym(s);
  loggable.append(stringified.sym);
  return SymExp(loggable);
}

SymExp colorPixel(SymExp args) {
  const uint8_t argSz = 5;
  SexpType argShape[] = {INTEGER, INTEGER, INTEGER, INTEGER, INTEGER};
  auto badShape = checkShape(argShape, argSz, args);
  if (badShape.type == NIL) {
    uint8_t argInts[argSz];
    auto argSexp = args.list.begin();
    for (int i = 0; i < argSz; i++) {
      argInts[i] = (uint8_t)argSexp->numInt;
      ++argSexp;
    }
    mkshft_ledMatrix::colorStripPixel(argInts[0], argInts[1], argInts[2],
                                      argInts[3], argInts[4]);
    // do color stuff
    return SymExp();
  } else {
    return error(MISMATCH_ARGUMENTS);
  }
}
SymExp unsafe_callUI(SymExp symexp) {
  // if (symexp.type == LIST && symexp.data.begin()->type == SYMBOL) {
  //   auto sexpItr = symexp.data.begin();
  // }
  return SymExp();
}
} // namespace mkshft_lisp
