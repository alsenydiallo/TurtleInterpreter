#ifndef PARSER_H
#define PARSER_H

#include "Scanner.h"
#include "AST.h"
#include <list>

class Parser {
private:
  Scanner& scanner_;
  std::list<Stmt*> AST_;  // list of top-level AST's created
  Token lookahead_;
  Attribute attribute_;
  int lineno_;
public:
  Parser(Scanner& s) : scanner_(s), AST_{}, lineno_(1) {}
  void parse(); // throws
  std::list<Stmt*>& syntaxTrees() {return AST_;}
private:
  void match(Token tok);
  void prog();
  void stmt_seq(); // executes (and deletes) each top-level statement
 
  Stmt *stmt();
  Stmt *assign();
  Stmt *block();
  Stmt *while_stmt();
  Stmt *if_stmt();
  Stmt *elsePart();
  Stmt *action();
  
  Expr *expr();
  Expr *term();
  Expr *factor();
  Expr *bool_();
  Expr *bool_term();
  Expr *bool_factor();
  Expr *cmp();
};  

#endif // PARSER_H
