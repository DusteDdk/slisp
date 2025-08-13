#include <format>
#include "fundamental.h"

std::string FTypeToString(enum FType ft) {
	switch(ft) {
        case FType::List:
            return "List";
        case FType::Number:
            return "Number";
        case FType::String:
            return "String";
        case FType::Empty:
            return "Empty";
        case FType::Error:
            return "Error";
        case FType::Loop:
            return "Loop";
        case FType::True:
            return "True";
        case FType::False:
            return "False";
        case FType::Variable:
            return "Variable";
        case FType::Expression:
            return "Expression";
	}
	return "Unknown";
}

FundamentalList::FundamentalList() : Fundamental(FType::List) {}
FundamentalList::FundamentalList(size_t iSize) : Fundamental(FType::List) {
    body.reserve(iSize);
}

std::string FundamentalList::toString()
{
    std::string result = "";
    for(FundamentalRef item : body) {
        result = std::format("{} {}", result, item->toString());
    }
    return std::format("[ {} ]", result);
}

Fundamental::Fundamental(FType typ) : t(typ) {};

int Fundamental::diff(FundamentalRef other) {
    return -1;
}


FundamentalEmpty::FundamentalEmpty() : Fundamental(FType::Empty) {}
std::string FundamentalEmpty::toString() {
    return "::EMPTY::";
}


FundamentalTrue::FundamentalTrue(FundamentalRef val) : Fundamental(FType::True), v(val) {}
std::string FundamentalTrue::toString() {
    return std::format("::True({})::", v->toString());
}




FundamentalFalse::FundamentalFalse(FundamentalRef val) : Fundamental(FType::False), v(val) {}
std::string FundamentalFalse::toString() {
    return std::format("::False({})::", v->toString());
}



FundamentalLoop::FundamentalLoop(FundamentalRef lastHead) : Fundamental(FType::Loop), lh(lastHead) {}
std::string FundamentalLoop::toString() {
    return "::@::";
}


FundamentalNumber::FundamentalNumber(long double num) : Fundamental(FType::Number), n(num) {};
std::string FundamentalNumber::toString() {
    return std::to_string(n);
}
int FundamentalNumber::diff(FundamentalRef other) {
    auto o = std::dynamic_pointer_cast<FundamentalNumber>(other);
    return (int)(n - o->n);
}



FundamentalString::FundamentalString(std::string str) : Fundamental(FType::String), s(str) {};
std::string FundamentalString::toString() {
    return s;
}
int FundamentalString::diff(FundamentalRef other) {
    auto o = std::dynamic_pointer_cast<FundamentalString>(other);
    return (o->s != s);
}


FundamentalVariableDefinition::FundamentalVariableDefinition(std::string txt): FundamentalString(txt) { t = FType::Variable; }
std::string FundamentalVariableDefinition::toString() {
    return std::format("::variableDecl(Name:{}, value: {})::", s, ((!v)?"::novalue::":v->toString()) );
}



FundamentalError::FundamentalError(std::string txt) : FundamentalString(txt) { t = FType::Error; }
std::string FundamentalError::toString() {
    return s;
}

FundamentalExpr::FundamentalExpr(std::shared_ptr<NodeCall> call): Fundamental(FType::Expression), exprCall(call) {}
std::string FundamentalExpr::toString() {
    return std::format("Expression '{}' with {} arguments", exprCall->name, exprCall->args.size());
}
