#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED
#include <string>
#include <memory>
#include <vector>
#include <map>

#include "parsey.h"
#include "fundamental.h"
#include "scopey.h"
#include "lfunctions.h"

class Interpreter {
private:
	std::map<std::string, LFuncRef> calls;

public:
	Interpreter();
	FundamentalRef doCall(std::shared_ptr<NodeCall> call, FundamentalRef h);
	ScopeCreep scopey;
	void run(NodeRef program);
	FundamentalRef mkFundamentalExpr(std::shared_ptr<NodeCall> call, FundamentalRef h);
	FundamentalRef descend(NodeRef n, FundamentalRef h, bool loopTokenValid = false);
	void addFunc(std::string name, LFuncRef fun);
};

#endif
