#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include "parsey.h"
#include "fundamental.h"
#include "scopey.h"

class Interpreter {
private:
	ScopeCreep scopey;
	FundamentalRef descend(NodeRef n, FundamentalRef h, bool loopTokenValid=false);
	FundamentalRef doCall(std::shared_ptr<NodeCall> call, FundamentalRef h);

public:
	Interpreter();
	void run(NodeRef program);
};

#endif
