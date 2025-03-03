#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED
#include <memory>
#include <vector>
#include "parsey.h"
#include "fundamental.h"
#include "scopey.h"

class Interpreter {
private:
	FundamentalRef doCall(std::shared_ptr<NodeCall> call, FundamentalRef h);
	std::vector<std::shared_ptr<FundamentalVariableDefinition>> varsWaitingForValue;
	FundamentalRef descendReturnValue(FundamentalRef v);

public:
	Interpreter();
	ScopeCreep scopey;
	void run(NodeRef program);
	FundamentalRef descend(NodeRef n, FundamentalRef h, bool loopTokenValid = false, bool noImpScopeChange=false);
};

#endif
