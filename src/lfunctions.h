
//LanguageFUNCtionS

#ifndef LFUNCS_H_INCLUDED
#define LFUNCS_H_INCLUDED

#include <memory>
#include "parsey.h"
#include "fundamental.h"

class LFunc {
public:
	std::string fname;
	LFunc(std::string n) : fname(n) {}
	virtual ~LFunc() = default;
	virtual FundamentalRef run(FundamentalRef head, std::vector<FundamentalRef> args) = 0;
	virtual FundamentalRef run(NodeIdent* ident, FundamentalRef head, std::vector<FundamentalRef> args) { return run(std::move(head), std::move(args)); };
};

using LFuncRef = std::shared_ptr<LFunc>;

#endif