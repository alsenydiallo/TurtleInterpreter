#include "Env.h"

float Env::get(const std::string& name) {
  auto iter = table_.find(name);
  if (iter != table_.end())
    return iter->second;
  put(name, 0.0); // insert into table if it doesn't exist
  return 0.0;
}
