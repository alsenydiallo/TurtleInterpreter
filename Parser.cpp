#include "Parser.h"
#include <sstream>
#include <stdexcept>

void Parser::match(Token tok) {
  if (tok != lookahead_) {
    std::stringstream ss;
    ss << "Unexpected token '" << tokenToString(lookahead_) << "', ";
    ss << "Expecting '" << tokenToString(tok) << "'";
    throw std::runtime_error(ss.str());
  }
  lookahead_ = scanner_.nextToken(attribute_, lineno_);
}

void Parser::parse() {
  lookahead_ = scanner_.nextToken(attribute_, lineno_);
  try {
    prog();
  } catch(const std::exception& error) {
    std::stringstream ss;
    ss << lineno_ << ": " << error.what();
    throw std::runtime_error(ss.str());
  }
}

void Parser::prog() {
  stmt_seq();
  match(Token::EOT);
}

void Parser::stmt_seq() {
  while (lookahead_ != Token::EOT) {
    Stmt *s = block();
    AST_.push_back(s);
  }
}

Stmt *Parser::stmt() {
  if(lookahead_ == Token::IDENT)
		return assign();
  else if(lookahead_ == Token::WHILE)
  	return while_stmt();
  else if(lookahead_ == Token::IF)
  	return if_stmt();
  else 
  	return action();
}

Stmt *Parser::assign() {
  std::string name = attribute_.s;
  match(Token::IDENT);
  match(Token::ASSIGN);
  Expr *e = expr();
  return new AssignStmt(name, e);
}

Stmt *Parser::block() {
  std::vector<Stmt*> stmtList;
  do{
  	Stmt *s = stmt();
  	stmtList.push_back(s);
  } while (  lookahead_ == Token::HOME 
  				|| lookahead_ == Token::PENUP
  				|| lookahead_ == Token::PENDOWN
  				|| lookahead_ == Token::FORWARD
  				|| lookahead_ == Token::LEFT
  				|| lookahead_ == Token::RIGHT
  				|| lookahead_ == Token::WHILE
  				|| lookahead_ == Token::PUSHSTATE
  				|| lookahead_ == Token::POPSTATE
  				|| lookahead_ == Token::IF
  				|| lookahead_ == Token::IDENT);
  return new BlockStmt(stmtList);
}

Stmt *Parser::while_stmt() {
  match(Token::WHILE);
  Expr *cond = bool_();
  match(Token::DO);
  Stmt *body = block();
  match(Token::OD);
  return new WhileStmt(cond, body);
}

Stmt *Parser::elsePart() {
  if(lookahead_ == Token::ELSIF){
  	match(Token::ELSIF);
  	Expr *cond = bool_();
  	match(Token::THEN);
  	Stmt *body = block();
  	Stmt *elspart = elsePart();
  	return new IfStmt(cond, body, elspart);
  }
  else if(lookahead_ == Token::ELSE){
  	match(Token::ELSE);
  	Stmt *body = block();
  	match(Token::FI);
  	return new ElseStmt(body);
  }
  else if(lookahead_ == Token::FI){
  	match(Token::FI);
  	return nullptr;
  }
  throw std::runtime_error("Expecting ELSIF, ELSE or FI\n");
}

Stmt *Parser::if_stmt() {
  match(Token::IF);
  Expr *cond = bool_();
  match(Token::THEN);
  Stmt *body = block();
  Stmt *elspart = elsePart();
  return new IfStmt(cond, body, elspart);
}

Stmt *Parser::action() {
  switch(lookahead_){
		case Token::HOME: match(Token::HOME); return new HomeStmt();
		case Token::PENUP: match(Token::PENUP); return new PenUpStmt();
		case Token::PENDOWN: match(Token::PENDOWN); return new PenDownStmt() ;
		case Token::FORWARD: match(Token::FORWARD); return new ForwardStmt(expr());
		case Token::LEFT: match(Token::LEFT); return new LeftStmt(expr());
		case Token::RIGHT: match(Token::RIGHT); return new RightStmt(expr());
		case Token::PUSHSTATE: match(Token::PUSHSTATE); return new PushStateStmt();
		case Token::POPSTATE: match(Token::POPSTATE); return new PopStateStmt();
		default:
			throw std::runtime_error("Expecting turtle action statement!\n");
	}
}

Expr *Parser::expr() {
  Expr *e = term();
  while (lookahead_ == Token::PLUS ||
	 lookahead_ == Token::MINUS) {
    const auto op = lookahead_;
    match(lookahead_);
    Expr *t = term();
    if (op == Token::PLUS)
      e = new AddExpr(e, t);
    else
      e = new SubExpr(e, t);
  }
  return e;
}

Expr *Parser::term() {
  Expr *e = factor();
	while(lookahead_ == Token::MULT || lookahead_ == Token::DIV){
		const auto op = lookahead_;
		match(lookahead_);
		Expr *f = factor();
		if(op == Token::MULT){
			e = new MulExpr(e,f);
		} else {
			e = new DivExpr(e,f);
		}
	}
	return e;
}

Expr *Parser::factor() {
  switch(lookahead_){
	case Token::PLUS: match(Token::PLUS); return factor();
	case Token::MINUS: match(Token::MINUS); return new NegExpr(factor());
	case Token::LPAREN: {
			match(Token::LPAREN);
			Expr *e = expr();
			match(Token::RPAREN);
			return e;
		}
	case Token::IDENT: {
			std::string name_ = attribute_.s;
			match(Token::IDENT);
			return new VarExpr(name_);
		}
	case Token::REAL: {
			const float val_ = attribute_.f;
			match(Token::REAL);
			return new ConstExpr(val_);
		}
	default:
		std::cout << tokenToString(lookahead_) << std::endl;
		throw std::runtime_error("Unexpected factor!\n");
	}
}

Expr *Parser::bool_() {
	Expr *b1 = bool_term();
	while(lookahead_ == Token::OR){
		match(Token::OR);
		Expr *b2 = bool_term();
		b1 = new ORExpr(b1, b2);
	}
	return b1;
}

Expr *Parser::bool_term() {
  Expr *f1 = bool_factor();
	while(lookahead_ == Token::AND){
		match(Token::AND);
		Expr *f2 = bool_factor();
		f1 = new ANDExpr(f1, f2);
	}
	return f1;
}

Expr *Parser::bool_factor() {
  	switch(lookahead_){
		case Token::NOT: match(Token::NOT); return new NOTExpr(bool_factor());
		case Token::LPAREN: 
			{
				match(Token::LPAREN);
				Expr *b = bool_();
				match(Token::RPAREN);
				return b;
			}
		default: Expr *e = cmp(); return e;
	}
}

Expr *Parser::cmp() {
  Expr *left_ = expr();
	//const auto op_ = lookahead_;
	//Expr *right_ = expr();
	switch(lookahead_){
		case Token::EQ: {
			match(Token::EQ); 
			Expr *right_ = expr();
			return new EQ(left_,right_);
		}
		case Token::NE: {
			match(Token::NE); 
			Expr *right_ = expr();
			return new NE(left_,right_);
		}
		case Token::LT: {
			match(Token::LT); 
			Expr *right_ = expr();
			return new LT(left_,right_);
		}
		case Token::LE: {
			match(Token::LE); 
			Expr *right_ = expr();
			return new LE(left_,right_);
		}
		case Token::GT: {
			match(Token::GT); 
			Expr *right_ = expr();
			return new GT(left_,right_);
		}
		case Token::GE: {
			match(Token::GE);
			Expr *right_ = expr(); 
			return new GE(left_,right_);
		}
		default:
			throw std::runtime_error("Unexpected Compare Operator!\n");
	}
}


