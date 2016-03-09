#ifndef ENV_H
#define ENV_H

#include <string>
#include <map>

class Env {
private:
  std::map<std::string, float> table_;
public:
  Env() : table_{} {}
  void put(const std::string& name, float v) {table_[name] = v;}
  float get(const std::string& name);
};

#endif // ENV_H


