#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "Scanner.h"

int main(int argc, char *argv[]) {
  
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <prog.turtle>" << std::endl;
    exit(1);
  }

  std::ifstream in(argv[1]);
  if (!in.is_open()) {
    std::cerr << "Unable to open '" << argv[1] << "'!" << std::endl;
    exit(2);
  }

  Attribute attr;
  Scanner scanner(in);
  Token tok;
  int lineno;

  try {
    while ((tok = scanner.nextToken(attr, lineno)) != Token::EOT) {
      switch(tok) {
      case Token::UNKNOWN: std::cout << "Unknown Token!"; break;
      case Token::EOT:     std::cout << "EOT"; break; 
      case Token::IDENT:   std::cout << "IDENT(" << attr.s << ")"; break;
      case Token::REAL:    
	std::cout << std::setprecision(6) 
		  << "REAL(" << attr.f << ")"; 
	  break;
      case Token::ASSIGN:  std::cout << "ASSIGN"; break;
      case Token::EQ:      std::cout << "EQ"; break;
      case Token::NE:      std::cout << "NE"; break;
      case Token::LE:      std::cout << "LE"; break;
      case Token::LT:      std::cout << "LT"; break;
      case Token::GE:      std::cout << "GE"; break;
      case Token::GT:      std::cout << "GT"; break;
      case Token::LPAREN:  std::cout << "LPAREN"; break;
      case Token::RPAREN:  std::cout << "RPAREN"; break;
      case Token::OR:      std::cout << "OR"; break;
      case Token::AND:     std::cout << "AND"; break;
      case Token::NOT:     std::cout << "NOT"; break;
      case Token::PLUS:    std::cout << "PLUS"; break;
      case Token::MINUS:   std::cout << "MINUS"; break;
      case Token::MULT:    std::cout << "MULT"; break;
      case Token::DIV:     std::cout << "DIV"; break;
      case Token::IF:      std::cout << "IF"; break;
      case Token::THEN:    std::cout << "THEN"; break;
      case Token::ELSIF:   std::cout << "ELSIF"; break;
      case Token::ELSE:    std::cout << "ELSE"; break;
      case Token::FI:      std::cout << "FI"; break;
      case Token::WHILE:   std::cout << "WHILE"; break;
      case Token::DO:      std::cout << "DO"; break;
      case Token::OD:      std::cout << "OD"; break;
      case Token::HOME:    std::cout << "HOME"; break;
      case Token::PENUP:   std::cout << "PENUP"; break;
      case Token::PENDOWN: std::cout << "PENDOWN"; break;
      case Token::FORWARD: std::cout << "FORWARD"; break;
      case Token::LEFT:    std::cout << "LEFT"; break;
      case Token::RIGHT:      std::cout << "RIGHT"; break;
      case Token::PUSHSTATE:  std::cout << "PUSHSTATE"; break;
      case Token::POPSTATE:   std::cout << "POPSTATE"; break;
      default:
	std::cerr << "UKNOWN TOKEN";
	exit(1);
      }
      std::cout << std::endl;
    }
  } catch (const std::exception& error) {
    std::cerr << error.what() << std::endl;
    exit(1);
  }

  return 0;
}
