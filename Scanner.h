#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <iostream>

enum class Token {
  UNKNOWN, EOT,
  IDENT, REAL, 
  ASSIGN, EQ, NE, LE, LT, GE, GT,
  LPAREN, RPAREN, OR, AND, NOT, PLUS, MINUS, MULT, DIV,
  IF, THEN, ELSIF, ELSE, FI,
  WHILE, DO, OD,
  HOME, PENUP, PENDOWN, FORWARD, LEFT, RIGHT, PUSHSTATE, POPSTATE
};

std::string tokenToString(Token token);

struct Attribute {  // should use variant type
  float f;          // just use simple struct for now
  std::string s;
};

class Scanner {
private:
  std::istream& in_;
  int lineno_;
public:
  Scanner(std::istream& in) : in_(in), lineno_{1} {}
  Token nextToken(Attribute& attr, int& lineno);
};

#endif // SCANNER_H
