#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <stdexcept>
#include "Scanner.h"
#include "Env.h"
#include "Parser.h"

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

  Scanner scanner(in);
  Parser parser(scanner);

  try {
    parser.parse();
  } catch (const std::exception& error) {
    std::cerr << error.what() << std::endl;
    exit(3);
  }

  std::list<Stmt*>& prog = parser.syntaxTrees();

  Env *env = new Env();

  try {
    for (Stmt *s : prog)
      s->execute(*env);
  } catch (const std::exception& error) {
    std::cerr << error.what() << std::endl;
    exit(4);
  }

  return 0;
}
