#include <ranges>
#include <functional>
#include <algorithm>
#include <string>
#include <ranges>

#include <format>
#include <print>
#include "scopey.h"

ScopeItem::ScopeItem(std::string n, ScopeStore* o) : owner(o), name(std::move(n)) {}

void ScopeItem::pop() {
    // Remove top element
    stack.pop();
    // If no elements left, remove itself from the storage
    if (stack.empty()) {
        owner->erase(name);
    }
}

void ScopeItem::push(FundamentalRef value) {
    stack.push(value);
}

void ScopeItem::set(FundamentalRef value) {
    // Overwrite the top
    if (stack.empty()) {
        throw std::runtime_error("Cannot set value on an empty stack for variable: " + name);
    }
    stack.top() = value;
}

FundamentalRef ScopeItem::read() {
    // Read the top of the stack
    if (stack.empty()) {
        throw std::runtime_error("Cannot read value from an empty stack for variable: " + name);
    }
    return stack.top();
}


std::string ScopeCreep::trace()
{
    //auto reverse_view = std::ranges::reverse_view{ stackTrace };
    std::string st = "::EMPTY_STACK::";
    auto it = stackTrace.begin();
    if (it != stackTrace.end()) {
        st = *it;
        while (++it != stackTrace.end()) {
            st += " | " + *it;
        }
    }
    return st;
}

FundamentalRef ScopeCreep::read(const std::string& varName) {
    // Safely look up varName
    auto it = storage.find(varName);
    if (it == storage.end()) {
        return std::make_shared<FundamentalError>(std::format("Cannot read variable '{}': Not in scope {}", varName, trace()));
    }
    return it->second.read();
}

std::string ScopeCreep::dump()
{
    std::string ret="";
    for (auto const& x : storage)
    {
        int stackIdx = x.second.stack.size() - 1;
        std::string stack = std::format( "[{}]{}", stackIdx, stackTrace.at(stackIdx));
        ret += std::format("{} / {}: {}\n", x.first, stack, x.second.stack.top()->toString() );

    }
    return ret;
}

std::string ScopeCreep::vars()
{
    std::string ret="Variables:\n";
    std::vector<std::array<std::string, 3>> cells;

    int l[] = { 0, 0 ,0 };
    for (auto const& x : storage)
    {
        int stackIdx = x.second.stack.size() - 1;
        std::string stack = std::format( "[{}]{}", stackIdx, stackTrace.at(stackIdx));
        //ret += std::format("Name: '{}'  Type: {}  Scope: {}\n", x.first,FTypeToString( x.second.stack.top()->t ), stack);
        std::array<std::string,3> cell( { x.first,FTypeToString( x.second.stack.top()->t ), stack } );
        cells.push_back( cell );
        for (auto [i, s] : std::views::enumerate(cell)) {
            int len = s.length();
            if(l[i] < len) {
                l[i] = len;
            }
        }
    }

    int width = l[0] + l[1] + l[2] + 10;
    auto s =std::string(width, '-') + "\n";


    ret += std::format(" {:<{}}   {:<{}}   {:<{}}\n",
        "Name", l[0],
        "Type", l[1],
        "Scope", l[2]);
    ret += s;


    for(auto const& cell : cells) {
        ret += std::format("| {:<{}} | {:<{}} | {:<{}} |\n",
            cell[0], l[0],
            cell[1], l[1],
            cell[2], l[2]);
    }
    ret += s;

    return ret;
}

FundamentalRef ScopeCreep::write(std::shared_ptr<FundamentalVariableDefinition> vd) {

    if (scopeStack.empty()) {
        return std::make_shared<FundamentalError>(std::format("Cannot write variable '{}': No scope.", vd->s));
    }

    FundamentalRef value = vd->v;
    std::string varName = vd->s;
    bool isKnownName = vd->isKnownName;

    if (!isKnownName) {
        return writeToScope(varName, value);
    }
    return writeToKnown(varName, value);
}

bool ScopeCreep::has(std::string& varName)
{
    auto it = storage.find(varName);
    return (it != storage.end());
}


FundamentalRef ScopeCreep::writeToScope(const std::string& varName, FundamentalRef value) {

    auto& currentScope = scopeStack.top();

    // Check if variable name already exists in the storage
    auto it = storage.find(varName);
    if (it != storage.end()) {
        // The variable already exists in some scope
        ScopeItem* itemPtr = &it->second;

        // Check if this item is already in the current scope
        auto found = std::find(currentScope.begin(), currentScope.end(), itemPtr);
        if (found != currentScope.end()) {
            // It's in the current scope - just overwrite
            itemPtr->set(value);
            return value;
        }

        // It's in an outer scope - shadow it by pushing a new value
        itemPtr->push(value);
        // Record that we introduced/shadowed this variable in the current scope
        currentScope.push_back(itemPtr);
    } else {
        // The variable doesn't exist at all, so create it
        auto result = storage.emplace(varName, ScopeItem(varName, &storage));
        ScopeItem& newItem = result.first->second;
        newItem.push(value);

        // Add a pointer to the newly created item in current scope
        currentScope.push_back(&newItem);
    }
    return value;
}

FundamentalRef ScopeCreep::writeToKnown(const std::string& varName, FundamentalRef value) {
    // Find if the variable is in storage
    auto it = storage.find(varName);
    if (it == storage.end()) {
        // The variable does not exist in any scope
        return std::make_shared<FundamentalError>(std::format("Cannot write known-variable '{}': not in scope {}", varName, trace()));
    }

    it->second.set(value);

    return value;
}



void ScopeCreep::enterScope(std::string n) {
        stackTrace.push_back(n);
        scopeStack.push(std::vector<ScopeItem*>{});
        //std::println(">> {}", trace());
}


void ScopeCreep::exitScope() {
    if (scopeStack.empty()) {
        std::println("Runtime error: Tried to exitScope beyond last scope.");
        return;
    }

    // Get the top scope (vector of pointers to ScopeItem)
    auto& currentScope = scopeStack.top();

    // Pop each variable that was introduced or shadowed in this scope
    for (auto var : currentScope) {
        var->pop();
    }

    // Finally pop the scope itself
    scopeStack.pop();
    stackTrace.pop_back();
    //std::println("<< {}", trace());
}
