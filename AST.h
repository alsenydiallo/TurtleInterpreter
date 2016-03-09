#ifndef AST_H
#define AST_H

#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include "Env.h"
#include <cmath>

#define MY_EPSILON 10e-6
//
// Abstract base class for all expressions.
//
class Expr {
public:
  virtual ~Expr() {}
  virtual float eval(Env& env) const = 0;
};

//
// Abstract base class for all statements.
//
class Stmt {
public:
  virtual ~Stmt() {};
  virtual void execute(Env& env) = 0;
};

//
// XXXX
// AST's expressions and statements go here.
//

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//  											Statements
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

class AssignStmt : public Stmt {
protected:
	std::string name_;
	Expr *expr_;
public:
	AssignStmt(const std::string& n, Expr *e): name_{n}, expr_{e} {}
	void execute(Env& env) {env.put(name_, expr_->eval(env));}
	~AssignStmt() {delete expr_;}
};

class WhileStmt : public Stmt {
protected:
	Expr *cond_;
	Stmt *body_;
public:
	WhileStmt(Expr *c, Stmt *b) : cond_{c}, body_{b} {}
	void execute(Env& env){
		while(cond_->eval(env) != 0){
			body_->execute(env);
		}
	}
	~WhileStmt(){ delete cond_; delete body_;}
};

class IfStmt : public Stmt {
protected:
	Expr *cond_;
	Stmt *body_;
	Stmt *elsPrt_;
public:
	IfStmt(Expr *c, Stmt *b, Stmt *els) : cond_{c}, body_{b}, elsPrt_{els} {}
	void execute(Env& env){
		if(cond_->eval(env) != 0){
			body_->execute(env);
		}else{
			if(elsPrt_ != nullptr)
				elsPrt_->execute(env);
		}
	}
	~IfStmt() {delete cond_; delete body_; if(elsPrt_ != nullptr) delete elsPrt_;}
};

class ElseStmt : public Stmt {
private:
	Stmt *body_;
public:
	ElseStmt(Stmt* b) : body_{b} {}
	void execute(Env& env){
		body_->execute(env);
	}
	~ElseStmt(){delete body_;}
};

class ForwardStmt : public Stmt {
protected:
  Expr *dist_;
public:
  ForwardStmt(Expr *e) : dist_{e} {}
  void execute(Env& env) {
    const float d = dist_->eval(env);
    std::cout << "M " << d << std::endl;
  }
  ~ForwardStmt() {delete dist_;}
};

class RightStmt : public Stmt {
protected:
  Expr *angle_;
public:
  RightStmt(Expr *e) : angle_{e} {}
  void execute(Env& env) {
    const float a = angle_->eval(env);
    std::cout << "R " << -a << std::endl;
  }
  ~RightStmt() {delete angle_;}
};

class LeftStmt : public Stmt {
protected:
  Expr *angle_;
public:
  LeftStmt(Expr *e) : angle_{e} {}
  void execute(Env& env) {
    const float a = angle_->eval(env);
    std::cout << "R " << a << std::endl;
  }
  ~LeftStmt() {delete angle_;}
};

class BlockStmt : public Stmt {
private: 
	std::vector<Stmt*> blockStmt_;
public:
	BlockStmt(std::vector<Stmt*> bS) : blockStmt_{bS} {}
	void execute(Env& env){
		for( auto stmt : blockStmt_)
			stmt->execute(env);
	}
};

class HomeStmt : public Stmt {
public:
  void execute(Env& env) {
    std::cout << "H" << std::endl;
  }
};

class PenUpStmt : public Stmt {
public:
  void execute(Env& env) {
    std::cout << "U" << std::endl;
  }
};

class PenDownStmt : public Stmt {
public:
  void execute(Env& env) {
    std::cout << "D" << std::endl;
  }
};

class PushStateStmt : public Stmt {
public:
  void execute(Env& env) {
    std::cout << "[" << std::endl;
  }
};

class PopStateStmt : public Stmt {
public:
  void execute(Env& env) {
    std::cout << "]" << std::endl;
  }
};


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// 															Expresions
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

class UnaryExpr : public Expr {
protected:
  Expr *expr_;
public:
  UnaryExpr(Expr *e) : expr_{e} {}
  ~UnaryExpr() {delete expr_;}
};

class BinaryExpr : public Expr {
protected:
  Expr *left_, *right_;
public:
  BinaryExpr(Expr *l, Expr *r) : left_{l}, right_{r} {}
  ~BinaryExpr() {delete left_; delete right_;}
};

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

class VarExpr : public Expr {
protected:
  const std::string name_;
public:
  VarExpr(const std::string& n) : name_{n} {}
  float eval(Env& env) const {
    return env.get(name_);
  }
};

class ConstExpr : public Expr {
protected:
  const float val_;
public:
  ConstExpr(float v) : val_{v} {}
  float eval(Env& env) const {
    return val_;
  }
};

class NegExpr : public UnaryExpr {
public:
  NegExpr(Expr *e) : UnaryExpr(e) {}
  float eval(Env& env) const {
    return -expr_->eval(env);
  }
};

class AddExpr : public BinaryExpr {
public:
  AddExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  float eval(Env& env) const {
    return left_->eval(env) + right_->eval(env);
  }
};

class SubExpr : public BinaryExpr {
public:
  SubExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  float eval(Env& env) const {
    return left_->eval(env) - right_->eval(env);
  }
};

class MulExpr : public BinaryExpr {
public:
  MulExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  float eval(Env& env) const {
    return left_->eval(env) * right_->eval(env);
  }
};

class DivExpr : public BinaryExpr {
public:
  DivExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  float eval(Env& env) const {
    return left_->eval(env) / right_->eval(env);
  }
};

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

class ORExpr: public BinaryExpr{
public:
	ORExpr(Expr *l, Expr *r) : BinaryExpr(l, r) {}
	float eval(Env &env) const { 
		return (left_->eval(env) == 0.0 && right_->eval(env) == 0.0 ) ? 0.0 : 1.0;
	}
};

class ANDExpr: public BinaryExpr{
public:
	ANDExpr(Expr *l, Expr *r) : BinaryExpr(l, r) {}
	float eval(Env &env) const {
		return (left_->eval(env) != 0.0 && right_->eval(env) != 0.0 ) ? 1.0 : 0.0;
	}
};

class NOTExpr : public UnaryExpr {
public: 
	NOTExpr(Expr *e) : UnaryExpr(e){}
	float eval(Env& env) const { return (expr_->eval(env) != 0.0) ? 0.0 : 1.0; }
};

class NE : public BinaryExpr {
public: 
	NE(Expr *l, Expr* r) : BinaryExpr(l,r) {}
	float eval(Env& env) const {
		return (left_->eval(env) != right_->eval(env)) ? 1.0 : 0.0;
	}
};

class LT : public BinaryExpr {
public: 
	LT(Expr *l, Expr *r) : BinaryExpr(l,r) {}
	float eval(Env& env) const {
		return (left_->eval(env) < right_->eval(env)) ? 1.0 : 0.0;
	}
};

class LE : public BinaryExpr {
public: 
	LE(Expr *l, Expr *r) : BinaryExpr(l,r) {}
	float eval(Env& env) const {
		return (left_->eval(env) <= right_->eval(env)) ? 1.0 : 0.0;
	}
};

class GT : public BinaryExpr {
public: 
	GT(Expr *l, Expr *r) : BinaryExpr(l,r) {}
	float eval(Env& env) const {
		return (left_->eval(env) > right_->eval(env)) ? 1.0 : 0.0;
	}
};

class GE : public BinaryExpr {
public: 
	GE(Expr *l, Expr *r) : BinaryExpr(l,r) {}
	float eval(Env& env) const {
		return (left_->eval(env) >= right_->eval(env)) ? 1.0 : 0.0;
	}
};

class EQ : public BinaryExpr {
public: 
	EQ(Expr *l, Expr *r) : BinaryExpr(l,r) {}
	float eval(Env& env) const {
		return (left_->eval(env) == right_->eval(env)) ? 1.0 : 0.0;
	}
};

#endif // AST_H
