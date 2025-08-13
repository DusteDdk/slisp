#ifndef SCOPEY_H_INCLUDED
#define SCOPEY_H_INCLUDED

#include <string>
#include <unordered_map>
#include <stack>
#include <vector>
#include <algorithm>
#include <memory>

#include "fundamental.h"


class ScopeItem;
using ScopeStore = std::unordered_map<std::string, ScopeItem>;

class ScopeItem {
    ScopeStore *owner;

public:
    ScopeItem(std::string n, ScopeStore* o);
    std::stack<FundamentalRef> stack;
    std::string name;
    void pop();
    void push(FundamentalRef value);
    void set(FundamentalRef value);
    FundamentalRef read();
};

class ScopeCreep {
    ScopeStore storage;
    std::stack<std::vector<ScopeItem*>> scopeStack;
    std::vector<std::string> stackTrace;


    FundamentalRef writeToKnown(const std::string& varName, FundamentalRef value);
    FundamentalRef writeToScope(const std::string& varName, FundamentalRef value);

public:
    ScopeCreep() = default;
    std::string trace();
    std::string dump();
    std::string vars();
    FundamentalRef read(const std::string& varName);
    FundamentalRef write(std::shared_ptr<FundamentalVariableDefinition> vd);
    bool has(std::string& varName);
    void enterScope(std::string n);
    void exitScope();
};

#endif // SCOPEY_H_INCLUDED
