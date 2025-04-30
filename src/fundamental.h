#ifndef FUNDAMENTAL_H_INCLUDED
#define FUNDAMENTAL_H_INCLUDED

#include <vector>
#include <memory>
#include <string>

#include "node.h"

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
	Expression,
};

class Fundamental;
using FundamentalRef = std::shared_ptr<Fundamental>;
class Fundamental {
public:
	FType t;
	Fundamental(FType typ);
	virtual ~Fundamental() = default;
	virtual std::string toString() = 0;
	virtual int diff(FundamentalRef other);
};

class FundamentalEmpty : public Fundamental {
public:
	FundamentalEmpty();
	std::string toString() override;
};

class FundamentalList : public Fundamental {
public:
	std::vector<FundamentalRef> body;
	FundamentalList();
	FundamentalList(size_t iSize);
	std::string toString() override;
};


class FundamentalExpr : public Fundamental {
	public:
	std::shared_ptr<NodeCall> exprCall;
	FundamentalExpr(std::shared_ptr<NodeCall> call);
	std::string toString() override;
};
class FundamentalTrue : public Fundamental {
public:
	FundamentalRef v;
	FundamentalTrue(FundamentalRef val);
	std::string toString() override;
};

class FundamentalFalse : public Fundamental {
public:
	FundamentalRef v;
	FundamentalFalse(FundamentalRef val);
	std::string toString() override;
};

class FundamentalLoop : public Fundamental {
public:
	FundamentalRef lh;
	FundamentalLoop(FundamentalRef lastHead);
	std::string toString() override;
};

class FundamentalNumber : public Fundamental {
public:
	long double n;
	FundamentalNumber(long double num);
	std::string toString() override;
	int diff(FundamentalRef other) override;
};

class FundamentalString : public Fundamental {
public:
	std::string s;
	FundamentalString(std::string str);
	std::string toString() override;
	int diff(FundamentalRef other) override;
};

class FundamentalVariableDefinition: public FundamentalString {
public:
	bool isKnownName = false;
	FundamentalRef v = nullptr;
	FundamentalVariableDefinition(std::string txt);
	std::string toString() override;
};


class FundamentalError: public FundamentalString {
public:
	FundamentalError(std::string txt);
	std::string toString() override;
};

#endif