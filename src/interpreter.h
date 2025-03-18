#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED
#include <memory>
#include <vector>
#include "parsey.h"
#include "fundamental.h"
#include "scopey.h"

class Interpreter {
private:
	std::vector<std::shared_ptr<FundamentalVariableDefinition>> varsWaitingForValue;

public:
	Interpreter();
	FundamentalRef doCall(std::shared_ptr<NodeCall> call, FundamentalRef h);
	ScopeCreep scopey;
	void run(NodeRef program);
	FundamentalRef descend(NodeRef n, FundamentalRef h, bool loopTokenValid = false);
};

#endif
