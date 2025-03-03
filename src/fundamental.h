#ifndef FUNDAMENTAL_H_INCLUDED
#define FUNDAMENTAL_H_INCLUDED

#include <vector>
#include <memory>
#include <string>

enum class FType : int_fast8_t {
	List=1,
	Number,
	String,
	Empty,
	Error,
	Loop,
	True,
	False,
	Variable,
};

class Fundamental;
using FundamentalRef = std::shared_ptr<Fundamental>;
class Fundamental {
public:
	FType t;
	Fundamental(FType typ) : t(typ) {};
	virtual ~Fundamental() = default;
	virtual std::string toString() = 0;
	virtual int diff(FundamentalRef other) { return -1; };
};

class FundamentalEmpty : public Fundamental {
public:
	FundamentalEmpty() : Fundamental(FType::Empty) {}
	std::string toString() override { return "::EMPTY::"; }
};

class FundamentalList : public Fundamental {
public:
	std::vector<FundamentalRef> body;
	FundamentalList() : Fundamental(FType::List) {}
	FundamentalList(size_t iSize) : Fundamental(FType::List) { body.reserve(iSize); }
	std::string toString() override;
};


class FundamentalTrue : public Fundamental {
public:
	FundamentalRef v;
	FundamentalTrue(FundamentalRef val) : Fundamental(FType::True), v(val) {}
	std::string toString() override { return std::format("::True({})::", v->toString()); }
};

class FundamentalFalse : public Fundamental {
public:
	FundamentalRef v;
	FundamentalFalse(FundamentalRef val) : Fundamental(FType::False), v(val) {}
	std::string toString() override { return std::format("::False({})::", v->toString()); }
};

class FundamentalLoop : public Fundamental {
public:
	FundamentalRef lh;
	FundamentalLoop(FundamentalRef lastHead) : Fundamental(FType::Loop), lh(lastHead) {}

	std::string toString() override { return "::@::"; }
};

class FundamentalNumber : public Fundamental {
public:
	long double n;
	FundamentalNumber(long double num) : Fundamental(FType::Number), n(num) {};
	std::string toString() override { return std::to_string(n); }
	int diff(FundamentalRef other) override { auto o = std::dynamic_pointer_cast<FundamentalNumber>(other); return (int)(n - o->n); };
};

class FundamentalString : public Fundamental {
public:
	std::string s;
	FundamentalString(std::string str) : Fundamental(FType::String), s(str) {};
	std::string toString() override { return s; }
	int diff(FundamentalRef other) override { auto o = std::dynamic_pointer_cast<FundamentalString>(other); return (o->s != s); };
};

class FundamentalVariableDefinition: public FundamentalString {
public:
	bool isKnownName = false;
	FundamentalRef v = nullptr;
	FundamentalVariableDefinition(std::string txt) : FundamentalString(txt) { t = FType::Variable; }
	std::string toString() override { return std::format("::variableDecl(Name:{}, value: {})::", s, ((!v)?"::novalue::":v->toString()) ); }
};


class FundamentalError: public FundamentalString {
public:
	FundamentalError(std::string txt) : FundamentalString(txt) { t = FType::Error; }
	std::string toString() override { return s; }
};




#endif